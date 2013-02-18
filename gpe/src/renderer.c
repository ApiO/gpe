#include "renderer.h"
#include <stdint.h>

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
  _3F32 translate;
  _3F32 scale;
  U32   vbo[3];
  U32   vao;
  U32   tex_id;
  U32   world_depth, local_depth;
  //char  user_data[50];
} gpe_render_item_t;

void _renderer_scene_to_render_item (rsx_mngr *r, gpe_scene_item_t *s, 
                                     gpe_render_item_t *ri);
void _renderer_draw();

#define _renderer_compar_item(a,b)						          \
          ((a).translate.z == (b).translate.z           \
	          ? (a).world_depth == (b).world_depth        \
				      ? (a).local_depth == (b).local_depth      \
					      ? (a).tex_id > (b).tex_id               \
					      : (a).local_depth > (b).local_depth     \
	            : (a).world_depth > (b).world_depth       \
	          : (a).translate.z > (b).translate.z)

typedef gpr_array_t(gpe_render_item_t) render_buffer;
static render_buffer _render_buffer;

void renderer_init()
{
  gpr_array_init(gpe_render_item_t, &_render_buffer, gpr_default_allocator);
}

void renderer_shutdown()
{
  gpr_array_destroy(&_render_buffer);
}

void renderer_draw(rsx_mngr *r, graphic_buffer *gb)
{
  unsigned int i;
  for(i=0; i< gb->size; i++)
  {
    gpe_render_item_t ri;
    _renderer_scene_to_render_item(r, &gb->data[i], &ri);
    gpr_array_push_back(gpe_render_item_t, &_render_buffer, ri);
  }

  gpr_mergesort(_render_buffer.data, _render_buffer.size, gpe_render_item_t, 
                _renderer_compar_item, gpr_default_allocator);

  _renderer_draw();

  gpr_array_resize(gpe_render_item_t, &_render_buffer, 0);
}

void _renderer_scene_to_render_item(rsx_mngr *r, gpe_scene_item_t *s, 
                                    gpe_render_item_t *ri)
{
  gpe_sprite_t *sprite = gpr_idlut_lookup(gpe_sprite_t, &r->sprites, 
                                          s->sprite_id);
  
  ri->vbo[0] =  sprite->vbo[0];
  ri->vbo[1] =  sprite->vbo[1];
  ri->vbo[2] =  sprite->vbo[2];
  
  ri->vao =  sprite->vao;

  ri->tex_id =  sprite->tex_id;
  ri->scale =   s->scale;
  ri->translate =   s->translate;
  ri->world_depth = s->world_depth;
  ri->local_depth = sprite->local_depth;
  //memcpy(ri->user_data, sprite->user_data, sizeof(sprite->user_data));
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
  GLubyte indicies[] = { 0,1,2, 2,3,0 };
  
  glGenBuffers(1, &s->vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, s->vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  
  glGenBuffers(1, &s->vbo[1]);
  glBindBuffer(GL_ARRAY_BUFFER, s->vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  
  glGenBuffers(1, &s->vbo[2]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
  /*
  //vao
  glGenVertexArrays(1, &s->vao);
  glBindVertexArray(s->vao);

    glGenBuffers(1, &s->vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  
    glGenBuffers(1, &s->vbo[1]);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  
    glGenBuffers(1, &s->vbo[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  
  glBindVertexArray(0);
  */

  {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
      char buffer[1024];
      sprintf(buffer, "\n----\nOpenGl error: %s\n\tline: %d\n\tfile: %s\n----\n", 
        glewGetErrorString(err), __LINE__, __FILE__);
      OutputDebugString(buffer);
      exit(EXIT_FAILURE); 
    }
  }

}

void renderer_destroy_vbo (gpe_sprite_t *s)
{
  glDeleteBuffers(3, s->vbo);
}

void _renderer_draw()
{
  I32 i, len;
  U32 crr_tex = 0;

  len = _render_buffer.size;

  glColor3f(1.f, 1.f, 1.f);
  for(i=0; i<len; i++)
  {
    gpe_render_item_t *r = &gpr_array_item(&_render_buffer, i);

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
      glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, r->vbo[2]); // for indices
      glVertexPointer( 2, GL_FLOAT, 0, 0 );

      glBindBuffer( GL_ARRAY_BUFFER, r->vbo[1] );
      glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
            
      glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
      
      glBindBuffer(GL_ARRAY_BUFFER, 0 );
      /*
      glBindVertexArray(r->vao);
      glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
      glBindVertexArray(0);
      */


    } glPopMatrix();
    {
      GLenum err = glGetError();
      if (err != GL_NO_ERROR)
      {
        char buffer[1024];
        sprintf(buffer, "\n----\nOpenGl error: %s\n\tline: %d\n\tfile: %s\n----\n", 
          glewGetErrorString(err), __LINE__, __FILE__);
        OutputDebugString(buffer);
        exit(EXIT_FAILURE); 
      }
    }
  }
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
}