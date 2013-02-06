#include "window_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

void _wm_debug_init_axes (window_manager *m, GLfloat height, GLfloat width);
void _wm_gl_init (int height, int width);
void _wm_set_fps (window_manager *w);


void _wm_set_fps (window_manager *m)
{ 
	double currentTime = glfwGetTime();
	if ((currentTime -  m->fps_util.t0Value) >= 1.0)
	{
		 m->fps_util.fps = (I32)( m->fps_util.fpsFrameCount / (currentTime -  m->fps_util.t0Value));
		 m->fps_util.fpsFrameCount = 0;
		 m->fps_util.t0Value = glfwGetTime();

    if( m->fps_util.last_fps ==  m->fps_util.fps) return;
    m->fps_util.last_fps =  m->fps_util.fps;
    {
      wchar_t fps[128];
      swprintf(fps, 128, L"FPS: %d",   m->fps_util.fps);
      font_system_text_set( m->fps_util.id, fps, ALIGN_TEXT_RIGHT);
    }
	}
	else
		 m->fps_util.fpsFrameCount++;
}

void _wm_gl_init (int height, int width)
{
  glViewport(0, 0, width, height);
  
  glClearColor( .1f, .1f, .1f, 0.0f );

  glShadeModel(GL_SMOOTH);                                // Enable Smooth Shading
  glClearDepth(1.0f);                                     // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);                                // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);                                 // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Really Nice Perspective Calculations

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0f, (GLfloat)width, (GLfloat)height, 0.0f,  -1.0f, 1.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
  glEnable(GL_TEXTURE_2D);
}

void window_manager_init (window_manager *m, char * title, int height, int width)
{
  m->running = GL_TRUE;
  m->restart = 0;
  m->display_fps = 0;
  m->display_axes = 0;
  m->width = width;
  m->height = height;
 
  if( !glfwInit() ) exit( EXIT_FAILURE );
  
  if( !glfwOpenWindow( width, height, 0,0,0,0,0,0, GLFW_WINDOW ) )
  {
    glfwTerminate();
    exit( EXIT_FAILURE );
  }

  glfwSetWindowTitle(title);
  
  _wm_gl_init(height, width);
  _wm_debug_init_axes(m, (GLfloat)height, (GLfloat)width);
  
  font_system_init(gpr_default_allocator);
  m->fps_util.id = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  m->fps_util.t0Value = glfwGetTime();;
  m->fps_util.fpsFrameCount = 0;
  m->fps_util.fps = 0;
  font_system_text_set( m->fps_util.id, L"FPS: 0", ALIGN_TEXT_RIGHT);
}

void window_manager_clear (window_manager *m)
{  
  glClear( GL_COLOR_BUFFER_BIT );
  glClear( GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  
  if(m->display_axes) glCallList(m->axes_cmd);
}

void window_manager_swapBuffers (window_manager *m)
{
  m->restart = !(!glfwGetKey( GLFW_KEY_ENTER ) && glfwGetWindowParam( GLFW_OPENED ));

  if(m->restart == 1) {
    m->running = 0;
    return;
  }
  if( m->display_fps)
  {
    _wm_set_fps(m);
    font_system_text_print( m->fps_util.id, 10, 0, DOCK_TEXT_TOP_RIGHT, (F32)m->height, (F32)m->width);
  }

  glfwSwapBuffers();

  m->running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
}

void window_manager_free (window_manager *m)
{
  font_system_free();
  glfwTerminate();
}

void _wm_debug_init_axes(window_manager *m, GLfloat height, GLfloat width)
{

  GLfloat axes_verticies[64] = { 
    0,0, 0,height/4,
    0,0, width/4,0,
    -width,height/2,   width,height/2,
    -width,-height/2,  width,-height/2,
    -width/2,height,   -width/2,-height,
    width/2,height,    width/2,-height,
    -width,0,           0,0,
    0,0,                0,-height,
    -.75f*width,height,  -.75f*width,-height,
    -.25f*width,height,  -.25f*width,-height,
    .25f*width,height,   .25f*width,-height,
    .75f*width,height,   .75f*width,-height,
    width,-.75f*height,  -width,-.75f*height,
    width,-.25f*height,  -width,-.25f*height,
    width,.25f*height,   -width,.25f*height,
    width,.75f*height,   -width,.75f*height 
  };
  m->axes_cmd = glGenLists(1);

  glNewList(m->axes_cmd, GL_COMPILE);
  {
    glDisable(GL_TEXTURE_2D);
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer(2, GL_FLOAT, 0, axes_verticies);
    glLineWidth(3);
    glColor3f(0.f, 1.f, 0.f); glDrawArrays(GL_LINES, 0, 2);
    glColor3f(1.f, 0.f, 0.f); glDrawArrays(GL_LINES, 2, 2);
    glLineWidth(1);
    glColor3f(.4f, .4f, .4f); glDrawArrays(GL_LINES, 4, 12);
    glColor3f(.2f, .2f, .2f); glDrawArrays(GL_LINES, 16, 16);

    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnable(GL_TEXTURE_2D);

  }glEndList();
}