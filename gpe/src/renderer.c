#include "renderer.h"

#define GLEW_STATIC
#include "GL/glew.h"
#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>

typedef struct
{
  F32 transform[3];
  GLuint tex_id;
  U32 vbo[2];
} gpe_render_item_t;

typedef gpr_array_t(gpe_render_item_t) render_buffer;

void _renderer_scene_to_render_item (rsx_mngr *r, gpe_scene_item_t *s, gpe_render_item_t *ri);
void _renderer_draw(render_buffer *rb);

void renderer_draw(rsx_mngr *r, graphic_buffer *gb)
{
  unsigned int i;
  gpr_tmp_allocator_2048_t ta;
  gpr_allocator_t *a = (gpr_allocator_t*)&ta;
  render_buffer rb;

  gpr_tmp_allocator_init(a, 2048);
  gpr_array_init(gpe_render_item_t, &rb, a);

  //feed du render buffer
  for(i=0; i< gb->size; i++)
  {
    gpe_render_item_t ri;
    _renderer_scene_to_render_item(r, &gb->data[i], &ri);
    gpr_array_push_back(gpe_render_item_t, &rb, ri);
  }

  //sort

  //Draw
  _renderer_draw(&rb);

  //clean
  gpr_array_destroy(&rb);
  gpr_tmp_allocator_destroy(a);
}

void _renderer_scene_to_render_item(rsx_mngr *r, gpe_scene_item_t *s, gpe_render_item_t *ri)
{
  gpe_sprite_t *sprite = gpr_idlut_lookup(gpe_sprite_t, &r->sprites, s->sprite_id);
  ri->tex_id = sprite->tex_id;
  ri->transform[0] = s->world_transform[0];
  ri->transform[1] = s->world_transform[1];
  ri->transform[2] = s->world_transform[2];
  ri->vbo[0] = sprite->vbo[0];
  ri->vbo[1] = sprite->vbo[1];
}

void renderer_init_vbo (gpe_sprite_t *s)
{
/*
  F32 verticies[12] = { 
    0,0, 
    0,s->tex_h, 
    s->tex_w,s->tex_h, 
    s->tex_w,s->tex_h, 
    s->tex_w,0,
    0,0
  };
  F32 tex_coord[12] = { 
    s->tex_x/s->width, s->tex_y/s->height, 
    s->tex_x/s->width, (s->tex_y + s->tex_h)/s->height, 
    (s->tex_x + s->tex_w)/s->width, (s->tex_y + s->tex_h)/s->height, 
    (s->tex_x + s->tex_w)/s->width, (s->tex_y + s->tex_h)/s->height, 
    (s->tex_x + s->tex_w)/s->width, s->tex_y/s->height, 
    s->tex_x/s->width, s->tex_y/s->height
  };
*/
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

void _renderer_draw(render_buffer *rb)
{
  I32 i, len;
  U32 crr_tex = 0;

  len = rb->size;

  glColor3f(1.f, 1.f, 1.f);
  for(i=0; i<len; i++)
  {
    gpe_render_item_t *r = &gpr_array_item(rb, i);
    if(crr_tex != r->tex_id)
    {
      crr_tex = r->tex_id;
      glBindTexture( GL_TEXTURE_2D, crr_tex);
    }
    glPushMatrix();
    {
      glTranslatef(r->transform[0], r->transform[1], 0.f);
		  glBindBuffer( GL_ARRAY_BUFFER, r->vbo[0] );
		  glVertexPointer( 2, GL_FLOAT, 0, 0 );		// Set The Vertex Pointer To The Vertex Buffer
		  glBindBuffer( GL_ARRAY_BUFFER, r->vbo[1] );
		  glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
	    glDrawArrays( GL_QUADS, 0, 4 );
    } glPopMatrix();
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}