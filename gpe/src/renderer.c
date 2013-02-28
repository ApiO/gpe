#include "renderer.h"
#include <stdint.h>

#define GLEW_STATIC
#include "GL/glew.h"
#if _WIN32
#include <windows.h>
#endif
#include <GL\GL.h>
#include "gpr_sort.h"
#include <SOIL\SOIL.h>

#include <stdio.h>

void _renderer_set_render_buffer (renderer *r, gpe_scene_item_t *s, gpe_render_item_t *ri);
void _renderer_draw(renderer *r);
void _print_gl_error();

#define _renderer_compar_item(a,b)						          \
          ((a).translate.z == (b).translate.z           \
	          ? (a).world_depth == (b).world_depth        \
				      ? (a).local_depth == (b).local_depth      \
					      ? (a).tex_id > (b).tex_id               \
					      : (a).local_depth > (b).local_depth     \
	            : (a).world_depth > (b).world_depth       \
	          : (a).translate.z > (b).translate.z)


void renderer_init(renderer *r, ressource_manager *rm)
{
  r->rm = rm;
  gpr_array_init(gpe_render_item_t, &r->render_buffer, gpr_default_allocator);
  gpr_idlut_init(renderer_graphic_t, &r->graphics, gpr_default_allocator);
  gpr_idlut_init(renderer_texture_t, &r->textures, gpr_default_allocator);
}

void renderer_shutdown(renderer *r)
{
  U32 i;
  gpr_array_destroy(&r->render_buffer);

  for(i=0; i<r->textures.num_items; i++)
    glDeleteBuffers(3, (gpr_idlut_begin(renderer_graphic_t, &r->graphics) + i)->vbo);
  gpr_idlut_destroy(renderer_graphic_t, &r->graphics);

  for(i=0; i<r->textures.num_items; i++)
    glDeleteTextures(1, &(gpr_idlut_begin(renderer_texture_t, &r->textures) + i)->tex_id);
  gpr_idlut_destroy(renderer_texture_t, &r->textures);
}

U64 renderer_init_graphic (renderer *r, gpe_graphic_t *g)
{
  return 0;
}

void renderer_destroy_graphic (renderer *r, U64 id)
{
}

U64 renderer_init_texture (renderer *r, char *path)
{
  renderer_texture_t tex;
  tex.tex_id = SOIL_load_OGL_texture (
      path, 
      SOIL_LOAD_AUTO, 
      SOIL_CREATE_NEW_ID, 
		  SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS );

  if(tex.tex_id == 0)
  {
    char buffer[1024];
        sprintf(buffer, "\n----\nSOIL - file not found : %s\n\tline: %d\n\tfile: %s\n----\n", 
      path, __LINE__, __FILE__);
    OutputDebugString(buffer);
    exit(EXIT_FAILURE); 
  }

  return gpr_idlut_add(renderer_texture_t, &r->textures, &tex);
}

void renderer_destroy_texture (renderer *r, U64 id)
{
  renderer_texture_t *tex = gpr_idlut_lookup(renderer_texture_t, &r->textures, id);
  glDeleteTextures(1, &tex->tex_id);
  gpr_idlut_remove(renderer_texture_t, &r->textures, id);
}



void renderer_draw (renderer *r, graphic_buffer *gb)
{
}

void _renderer_set_render_buffer (renderer *r, gpe_scene_item_t *s, gpe_render_item_t *ri)
{
}

void _renderer_draw(renderer *r)
{
}

void _print_gl_error()
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

/*
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
*/