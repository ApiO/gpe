#include "window_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

//static void _DEV_drawAxes(void);
void _wm_debug_init_axes (window_manager *m, GLfloat height, GLfloat width);
void _wm_debug_draw_axes (window_manager *m);
void _wm_gl_init (int height, int width);
void _wm_set_fps (window_manager *manager);

double t0Value; // Set the initial time to now
int    fpsFrameCount;

void _wm_set_fps (window_manager *m)
{ 
	double currentTime = glfwGetTime();
	if ((currentTime - t0Value) > 1.0)
	{
		m->fps = (double)fpsFrameCount / (currentTime - t0Value);
    
		fpsFrameCount = 0;
		t0Value = glfwGetTime();
	}
	else
		fpsFrameCount++;
}

void _wm_gl_init (int height, int width)
{
  glViewport(0, 0, width, height);
  
  glClearColor( .1f, .1f, .1f, 0.0f );

  glShadeModel(GL_SMOOTH);							  // Enable Smooth Shading
	glClearDepth(1.0f);									    // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							  // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								  // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

  glOrtho(0.0f, (GLfloat)width, (GLfloat)height, 0.0f,  -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void window_manager_init (window_manager *m, char * title, int height, int width)
{
  m->running = GL_TRUE;
  m->display_fps = 0;
  m->display_axes = 0;
  m->restart = 0;
  m->fps = 0.0;
 
  if( !glfwInit() ) exit( EXIT_FAILURE );
  
  if( !glfwOpenWindow( width, height, 0,0,0,0,0,0, GLFW_WINDOW ) )
  {
    glfwTerminate();
    exit( EXIT_FAILURE );
  }

  glfwSetWindowTitle(title);
  
  _wm_gl_init(height, width);
  _wm_debug_init_axes(m, (GLfloat)height, (GLfloat)width);

  glbmfont_load();

  t0Value = glfwGetTime();;
  fpsFrameCount = 0;
}

void window_manager_clear (window_manager *m)
{  
  glClear( GL_COLOR_BUFFER_BIT );
  glClear( GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  
  if(m->display_axes) _wm_debug_draw_axes(m);
}

void window_manager_swapBuffers (window_manager *m)
{
  m->restart = !(!glfwGetKey( GLFW_KEY_ENTER ) && glfwGetWindowParam( GLFW_OPENED ));

  if(m->restart == 1) {
    m->running = 0;
    return;
  }

  if(m->display_fps)
  {
    char buffer[255];
    _wm_set_fps(m);
    sprintf_s(buffer, "FPS: %.0f", m->fps);
    glbmfont_print(buffer, 5, 0, dock_top_right);
  }

  glfwSwapBuffers();

  m->running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
}


void window_manager_print (window_manager *m, char * t, int x, int y, gpe_dock d)
{
  glbmfont_print(t, x, y, d);
}

void window_manager_free (window_manager *m)
{
  glfwTerminate();
  glbmfont_free();
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

   memcpy(m->axes_verticies, axes_verticies, sizeof(axes_verticies));
}

void _wm_debug_draw_axes (window_manager *m)
{
    glEnableClientState( GL_VERTEX_ARRAY );
    
    glPushMatrix();
    {
      glVertexPointer(2, GL_FLOAT, 0, m->axes_verticies);
      glLineWidth(3);
      glColor3f(0.f, 1.f, 0.f); glDrawArrays(GL_LINES, 0, 2);
      glColor3f(1.f, 0.f, 0.f); glDrawArrays(GL_LINES, 2, 2);
      glLineWidth(1);
      glColor3f(.4f, .4f, .4f); glDrawArrays(GL_LINES, 4, 12);
      glColor3f(.2f, .2f, .2f); glDrawArrays(GL_LINES, 16, 16);
    } glPopMatrix();
  
    glDisableClientState( GL_VERTEX_ARRAY );
}
