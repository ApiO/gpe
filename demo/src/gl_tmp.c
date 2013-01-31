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


typedef struct
{
  GLint tex_coord[8];
  GLfloat verticies[8];
} gp_t_tmp;


typedef struct
{
	I32     x, y, h, w;
  F64     x_off, y_off;
} gp_t_chr;

void _t_init__(gp_t_tmp *text, gp_t_chr *chr);


void GL_TMP_H_FOO()
{
  U32             tex_id;
  window_manager  w;
  gp_t_tmp  t;
  gp_t_chr  chr;

  gpr_memory_init(410241024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_fps = false;
  w.display_axes = true;

  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  chr.h = TEX_HEIGHT;
  chr.w = TEX_WIDTH;
  chr.x = 0;
  chr.y = 0;
  chr.x_off = 0;
  chr.y_off = 0;

  _t_init__(&t, &chr);

  if(tex_id != 0)
  {
    while(w.running)
    {
      window_manager_clear(&w);
      
        glColor3f(1.f, 1.f, 1.f);
        glBindTexture(GL_TEXTURE_2D, tex_id);  
        glTexCoordPointer(2, GL_INT, 0, t.tex_coord);
        glVertexPointer(2, GL_FLOAT, 0, t.verticies);
        glDrawArrays(GL_QUADS, 0, 4);
      
      window_manager_swapBuffers(&w);
    }
    
    glDeleteTextures(1, &tex_id);
  }

  window_manager_clear(&w);
  gpr_memory_shutdown();
}

void _t_init__(gp_t_tmp *text, gp_t_chr *chr)
{
  U32 tmp = 0;
  F32 line_pad_x = 0;
  F32 line_pad_y = 0;

  text->tex_coord[tmp] =   chr->x;  
  text->tex_coord[tmp+1] = chr->y;
  text->tex_coord[tmp+2] = chr->x;
  text->tex_coord[tmp+3] = chr->y + chr->h;
  text->tex_coord[tmp+4] = chr->x + chr->w;
  text->tex_coord[tmp+5] = chr->y + chr->h;
  text->tex_coord[tmp+6] = chr->x + chr->w;
  text->tex_coord[tmp+7] = chr->y;

  /*
  text->tex_coord[tmp] =   (GLfloat)chr->x/TEX_WIDTH;  
  text->tex_coord[tmp+1] = (GLfloat)chr->y/TEX_HEIGHT;
  text->tex_coord[tmp+2] = (GLfloat)chr->x/TEX_WIDTH;
  text->tex_coord[tmp+3] = (GLfloat)(chr->y + chr->h)/TEX_HEIGHT;
  text->tex_coord[tmp+4] = (GLfloat)(chr->x + chr->w/TEX_WIDTH);
  text->tex_coord[tmp+5] = (GLfloat)(chr->y + chr->h)/TEX_HEIGHT;
  text->tex_coord[tmp+6] = (GLfloat)(chr->x + chr->w)/TEX_WIDTH;
  text->tex_coord[tmp+7] = (GLfloat)chr->y/TEX_HEIGHT;
  */

  text->verticies[tmp] =   (GLfloat)chr->x_off+line_pad_x;    
  text->verticies[tmp+1] = (GLfloat)chr->y_off+chr->h+line_pad_y;
  text->verticies[tmp+2] = (GLfloat)chr->x_off+line_pad_x;
  text->verticies[tmp+3] = (GLfloat)chr->y_off+line_pad_y;
  text->verticies[tmp+4] = (GLfloat)chr->x_off+chr->w+line_pad_x;
  text->verticies[tmp+5] = (GLfloat)chr->y_off+line_pad_y;
  text->verticies[tmp+6] = (GLfloat)chr->x_off+chr->w+line_pad_x;
  text->verticies[tmp+7] = (GLfloat)chr->y_off+chr->h+line_pad_y;
  /*
  line_pad_x = TEX_WIDTH*2;
  line_pad_y = TEX_HEIGHT;
  tmp = 8;

  text->tex_coord[tmp] =   (GLfloat)chr->x;  
  text->tex_coord[tmp+1] = (GLfloat)chr->y;
  text->tex_coord[tmp+2] = (GLfloat)chr->x;
  text->tex_coord[tmp+3] = (GLfloat)chr->y + chr->h;
  text->tex_coord[tmp+4] = (GLfloat)chr->x + chr->w;
  text->tex_coord[tmp+5] = (GLfloat)chr->y + chr->h;
  text->tex_coord[tmp+6] = (GLfloat)chr->x + chr->w;
  text->tex_coord[tmp+7] = (GLfloat)chr->y;

  text->verticies[tmp] =   (GLfloat)chr->x_off+line_pad_x;    
  text->verticies[tmp+1] = (GLfloat)chr->y_off+chr->h+line_pad_y;
  text->verticies[tmp+2] = (GLfloat)chr->x_off+line_pad_x;
  text->verticies[tmp+3] = (GLfloat)chr->y_off+line_pad_y;
  text->verticies[tmp+4] = (GLfloat)chr->x_off+chr->w+line_pad_x;
  text->verticies[tmp+5] = (GLfloat)chr->y_off+line_pad_y;
  text->verticies[tmp+6] = (GLfloat)chr->x_off+chr->w+line_pad_x;
  text->verticies[tmp+7] = (GLfloat)chr->y_off+chr->h+line_pad_y;
  */
}


/*
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
}
*/