#include "gl_tmp.h"

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>
#include <SOIL\SOIL.h>
#include <GL\glfw.h>

#include "gpr_types.h"
#include "gpr_array.h"

#include "window_manager.h"

#define HEIGHT      600
#define WIDTH       800
#define ITEM_COUNT  42
#define TEX_HEIGHT  128
#define TEX_WIDTH   128

typedef F64 Matrix[4][4];

typedef struct
{
  Matrix  world_transform;
  F32     x, y;
  F32     scale_x, scale_y, scale_z;
  GLuint  cmd_id;
  GLuint  tex_id;
} graph;

typedef gpr_array_t(graph) graph_buffer;

static F64 _dev_clock_diff(clock_t start, clock_t end);
static void _draw_sprite (GLuint tex_id);
static void _draw_scene (graph_buffer *gb);
static void _sprite_to_scene (graph *g);

void _print_textured_vertex_array(graph_buffer *gb, GLuint tex_id);

static GLfloat verticies[] = { 0,TEX_HEIGHT,0, 0,0,0, TEX_WIDTH,0,0, TEX_WIDTH,TEX_HEIGHT,0 };
static GLfloat tex_coord[] = { 0,1, 0,0, 1,0, 1,1 };

void GL_TMP_H_FOO()
{
  int i;
  U32             tex_id;
  window_manager  w;
  GLuint          sprite_cmd;
  graph_buffer    gb;
  clock_t         start;
  
  gpr_memory_init(410241024);
  gpr_array_init(graph, &gb, gpr_default_allocator);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_fps = false;
  w.display_axes = true;

 

  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  for (i=0; i < ITEM_COUNT; i ++)
  {
    graph g;
    g.x = (F32)((WIDTH/2)*.5f*cos(2*M_PI/ITEM_COUNT*i));
    g.y = (F32)((HEIGHT/2)*.5f*sin(2*M_PI/ITEM_COUNT*i)); 
    g.scale_x = 1.f;
    g.scale_y = 1.f;
    g.scale_z = 1.f;
    g.tex_id = tex_id;
    gpr_array_push_back(graph, &gb, g);
  }

  if(tex_id != 0)
  {
    while(w.running)
    {
      window_manager_clear(&w);
      
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();

      glPushMatrix();
      {
        glTranslatef(-64+WIDTH/2, -64+HEIGHT/2, .0f);
        _print_textured_vertex_array(&gb, tex_id);
      } glPopMatrix();

      window_manager_swapBuffers(&w);
    }
    
    glDeleteTextures(1, &tex_id);
  }

  window_manager_clear(&w);
  gpr_array_destroy(&gb);
  gpr_memory_shutdown();
}



static F64 _dev_clock_diff(clock_t start, clock_t end)
{
  return (F64)(end - start) / CLOCKS_PER_SEC;
}

void _print_textured_vertex_array(graph_buffer *gb, GLuint tex_id)
{
  int i;

  glEnableClientState( GL_VERTEX_ARRAY );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex_id);  

  glColor3f(1.f, 1.f, 1.f);

  for (i=0; i < ITEM_COUNT; i ++)
  {
    graph *g = &gpr_array_item(gb, i);
    
    glPushMatrix();
    {
      glTranslatef(g->x, g->y, .0f);
	    glRotatef(.0f, .0f, .0f, .0f);
	    glScalef( g->scale_x, g->scale_y, g->scale_z);

      glTexCoordPointer(2, GL_FLOAT, 0, tex_coord);
      glVertexPointer(3, GL_FLOAT, 0, verticies);

      glDrawArrays(GL_QUADS, 0, 4);

    } glPopMatrix();
    
  }   
      
  glDisable(GL_TEXTURE_2D);

  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glDisableClientState( GL_VERTEX_ARRAY );
}
