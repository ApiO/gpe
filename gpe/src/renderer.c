#include "renderer.h"

#define GLEW_STATIC
#include "GL/glew.h"
#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>
#include "gpr_sort.h"

#include <stdio.h>

typedef struct
{
  _3F32   translate;
  _3F32   scale;
  GLuint  tex_id;
  U32     vbo[2];
  I32     group_id;
  F32     weight;
  vec2F32 aabb[2];
  wchar_t userData[50];
} gpe_render_item_t;

typedef struct
{
  U32 max_tex_id;
  gpr_array_t(gpe_render_item_t) buffer;
} gpe_render_buffer_t;


void _renderer_scene_to_render_item (rsx_mngr *r, gpe_scene_item_t *s, gpe_render_item_t *ri);
void _renderer_draw();
void _renderer_sort_buffer();

#define _renderer_compar_weight(a,b) ((a).weight < (b).weight)
#define _renderer_compar_z(a,b) ((a).translate.z > (b).translate.z)

static gpe_render_buffer_t _render_buffer;

void renderer_init()
{
  _render_buffer.max_tex_id = 0;
  gpr_array_init(gpe_render_item_t, &_render_buffer.buffer, gpr_default_allocator);
}

void renderer_shutdown()
{
  gpr_array_destroy(&_render_buffer.buffer);
}

void renderer_draw(rsx_mngr *r, graphic_buffer *gb)
{
  unsigned int i;
  //feed du render buffer
  for(i=0; i< gb->size; i++)
  {
    gpe_render_item_t ri;
    _renderer_scene_to_render_item(r, &gb->data[i], &ri);
    gpr_array_push_back(gpe_render_item_t, &_render_buffer.buffer, ri);
    if (_render_buffer.max_tex_id < ri.tex_id) _render_buffer.max_tex_id = ri.tex_id;
  }

  //sort
  _renderer_sort_buffer();
  _render_buffer.max_tex_id = 0;

  //Draw
  _renderer_draw();

  gpr_array_resize(gpe_render_item_t, &_render_buffer.buffer, 0);
}

void _renderer_scene_to_render_item(rsx_mngr *r, gpe_scene_item_t *s, gpe_render_item_t *ri)
{
  gpe_sprite_t *sprite = gpr_idlut_lookup(gpe_sprite_t, &r->sprites, s->sprite_id);
  ri->tex_id = sprite->tex_id;
  ri->translate = s->translate;
  ri->scale = s->scale;
  
  ri->aabb[0] = s->aabb[0];
  ri->aabb[1] = s->aabb[1];

  memcpy(ri->userData, s->userData, sizeof(s->userData));

  ri->vbo[0] = sprite->vbo[0];
  ri->vbo[1] = sprite->vbo[1];
  ri->group_id = -1;
}

void renderer_init_vbo (gpe_sprite_t *s)
{
  F32 verticies[8] = { 
    0,0, 
    0,s->tex_h, 
    s->tex_w,s->tex_h, 
    s->tex_w,0
  };
  F32 tex_coord[8] = { 
    s->tex_x/s->width, s->tex_y/s->height, 
    s->tex_x/s->width, (s->tex_y + s->tex_h)/s->height,
    (s->tex_x + s->tex_w)/s->width, (s->tex_y + s->tex_h)/s->height, 
    (s->tex_x + s->tex_w)/s->width, s->tex_y/s->height
  };

	glGenBuffers( 1, &s->vbo[0] );							                                              // Get A Valid Name
	glBindBuffer( GL_ARRAY_BUFFER, s->vbo[0] );			                                      // Bind The Buffer
  glBufferData( GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);  // Load The Data
	glGenBuffers( 1, &s->vbo[1] );
  glBindBuffer( GL_ARRAY_BUFFER, s->vbo[1] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderer_destroy_vbo (gpe_sprite_t *s)
{
  glDeleteBuffers(2, s->vbo);
}

void _renderer_draw()
{
  I32 i, len;
  U32 crr_tex = 0;

  len = _render_buffer.buffer.size;

  glColor3f(1.f, 1.f, 1.f);
  for(i=0; i<len; i++)
  {
    gpe_render_item_t *r = &gpr_array_item(&_render_buffer.buffer, i);
    if(crr_tex != r->tex_id)
    {
      crr_tex = r->tex_id;
      glBindTexture( GL_TEXTURE_2D, crr_tex);
    }
    glPushMatrix();
    {
      glTranslatef(r->translate.x, r->translate.y, 0.f);
      glScalef(r->scale.x, r->scale.y, r->scale.z);

		  glBindBuffer( GL_ARRAY_BUFFER, r->vbo[0] );
		  glVertexPointer( 2, GL_FLOAT, 0, 0 );		// Set The Vertex Pointer To The Vertex Buffer
		  glBindBuffer( GL_ARRAY_BUFFER, r->vbo[1] );
		  glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
	    glDrawArrays( GL_QUADS, 0, 4 );
    } glPopMatrix();
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static I32 _print = 0;
void _renderer_sort_buffer()
{
  //_render_buffer
  int i, j, start_index = 0;
  I32 len = (I32)_render_buffer.buffer.size;
  I32 curr_group = 1;

  //on tri par z
  gpr_mergesort(_render_buffer.buffer.data, _render_buffer.buffer.size, gpe_render_item_t, _renderer_compar_z, gpr_default_allocator);

  //on créé les groupes
  do
  {
    I32 exit = 1;
    //on prend le premier élément de la liste qui n'a pas de groupe
    for(i=start_index; i<len; i++)
      if(gpr_array_item(&_render_buffer.buffer, i).group_id == -1)
      { 
        //on lui donne l'id du groupe en cours
        gpr_array_item(&_render_buffer.buffer, i).group_id = curr_group;
        start_index=i;
        exit = 0;
        break;
      }
    if(exit) break;

    for(i=start_index+1; i<len; i++)
    {
      I32 overlap = 0;
      gpe_render_item_t *curr_item = &gpr_array_item(&_render_buffer.buffer, i);
      if(curr_item->group_id != -1) continue;
      
      for(j=start_index; j<i; j++)
      {
        gpe_render_item_t *tmp = &gpr_array_item(&_render_buffer.buffer, j);
        if(!(tmp->group_id == curr_group || tmp->group_id == -1)) continue;

        if(!((curr_item->aabb[0].x > tmp->aabb[1].x) || (tmp->aabb[0].x > curr_item->aabb[1].x) ||
             (curr_item->aabb[0].y > tmp->aabb[1].y) || (tmp->aabb[0].y > curr_item->aabb[1].y)))
        {
          overlap = 1;
          break;
        }
      }

      if(!overlap) curr_item->group_id = curr_group;
    }
    curr_group += 1;
  } while (1);
  
  if(!_print)
  {
    printf("\nGROUPS\n");
    for(i=0; i<len; i++)
    {
      gpe_render_item_t *tmp = &gpr_array_item(&_render_buffer.buffer, i); 
      wprintf(L"ud:%s\tg:%d\n", tmp->userData, tmp->group_id);
    }
  }

  //on update les poids
  curr_group -= 1;
  for(i=0; i<len; i++)
  {
    gpe_render_item_t *tmp = &gpr_array_item(&_render_buffer.buffer, i); 
    tmp->weight = ((F32)((I32)tmp->group_id)/curr_group)*_render_buffer.max_tex_id + (F32)tmp->tex_id/_render_buffer.max_tex_id;
  }

  //on tri
  gpr_mergesort(_render_buffer.buffer.data, _render_buffer.buffer.size, gpe_render_item_t, _renderer_compar_weight, gpr_default_allocator);
  
  if(!_print)
  {
    printf("\nWEIGHT SORT\n");
    for(i=0; i<len; i++)
    {
      gpe_render_item_t *tmp = &gpr_array_item(&_render_buffer.buffer, i); 
      wprintf(L"ud:%s\tg:%d\tt:%d\tw:%.4f\n", tmp->userData, tmp->group_id, (I32)tmp->tex_id, tmp->weight);
    }
  }
  if(!_print) _print = 1;
}
