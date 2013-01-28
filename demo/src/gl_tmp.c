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

#define HEIGHT      900
#define WIDTH       900
#define ITEM_COUNT  4
#define TEX_HEIGHT  128
#define TEX_WIDTH   128

typedef F64 Matrix[4][4];

typedef struct
{
  Matrix  world_transform;
  F32     x, y;
  F32     scale_x, scale_y;
  GLuint  cmd_id;
  GLuint  tex_id;
} graph;

typedef gpr_array_t(graph) graph_buffer;

static F64 _dev_clock_diff(clock_t start, clock_t end);
static void _draw_sprite (GLuint tex_id);
static void _draw_scene (graph_buffer *gb);
static void _sprite_to_scene (graph *g);

void GL_TMP_H_FOO()
{
  int i;
  U32             tex_id;
  window_manager  w;
  graph_buffer    gb;
  GLuint          sprite_cmd;
  clock_t         start;
  char            buffer[64];
  
  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_fps = true;
  w.display_axes = true;
  
  gpr_array_init(graph, &gb, gpr_default_allocator);
  
  glViewport(0, 0, WIDTH, HEIGHT);

  //load rsx
  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_INVERT_Y );

  //sprite draw cmd creation
  sprite_cmd = glGenLists(1);
  glNewList(sprite_cmd, GL_COMPILE);
  {
    _draw_sprite(tex_id);
  } glEndList();

  for(i=0; i < ITEM_COUNT; i++)
  {
    graph g;
    g.x = (F32)(0.5f*cos(2*M_PI/ITEM_COUNT*i)); 
    g.y = (F32)(0.5f*sin(2*M_PI/ITEM_COUNT*i)); 
    g.scale_x = (F32)TEX_WIDTH/WIDTH;
    g.scale_y = (F32)TEX_HEIGHT/HEIGHT;
    g.tex_id = tex_id;
    g.cmd_id = sprite_cmd;
    gpr_array_push_back(graph, &gb, g);
  }

  if(tex_id != 0)
  {
    while(w.running)
    {
      window_manager_clear(&w);

      start = clock();
      _draw_scene(&gb);
      sprintf(buffer, "%d graphics\nms: %.0f", ITEM_COUNT, _dev_clock_diff(start, clock())*1000);  
      window_manager_print(&w, buffer, 0, 0, dock_top_left);
      
      window_manager_swapBuffers(&w);
    }
    
    glDeleteLists(sprite_cmd, 1);
    glDeleteTextures(1, &tex_id);
  }

  window_manager_clear(&w);
  gpr_array_destroy(&gb);
  gpr_memory_shutdown();
}

static void _draw_scene(graph_buffer *gb)
{
  int i;
 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, gb->data[0].tex_id);  

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor3f(1.f, 1.f, 1.f);

  for (i=0; i < gb->size; i ++)
    _sprite_to_scene(&gpr_array_item(gb, i));

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glBlendFunc(GL_NONE, GL_NONE);
}

static void _sprite_to_scene(graph *g)
{
  glPushMatrix();
  {
    glTranslatef(g->x, g->y, .0f);
	  glRotatef(.0f, .0f, .0f, .0f);
	  glScalef( g->scale_x, g->scale_y, .0f);

    glCallList(g->cmd_id);

  } glPopMatrix();
}

static void _draw_sprite(GLuint tex_id)
{
  glBegin(GL_QUADS);
  {
 	  glTexCoord2f( 0, 1 );
 	  glVertex2d( -1, 1 );
      
 	  glTexCoord2f( 0, 0 );
 	  glVertex2d( -1, -1 );
      
 	  glTexCoord2f( 1, 0 );
 	  glVertex2d( 1, -1 );
 
 	  glTexCoord2f( 1, 1 );
 	  glVertex2d( 1, 1 );
	} 
  glEnd();
}

static F64 _dev_clock_diff(clock_t start, clock_t end)
{
  return (F64)(end - start) / CLOCKS_PER_SEC;
}
