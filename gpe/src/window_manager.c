#include "window_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

//static void _DEV_drawAxes(void);
static void _debug_init(window_manager *m);
void _window_manager_gl_init (int height, int width);
void _window_manager_set_fps (window_manager *manager);

double t0Value; // Set the initial time to now
int    fpsFrameCount;

void _window_manager_set_fps (window_manager *m)
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

void _window_manager_gl_init (int height, int width)
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
  
  _window_manager_gl_init(height, width);
  _debug_init(m);

  glbmfont_load();

  t0Value = glfwGetTime();;
  fpsFrameCount = 0;
}

void window_manager_clear (window_manager *m)
{  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  
  if(m->display_axes) glCallList(m->axes_cmd);

  if(!m->display_fps) return;  
  
  char buffer[255];
  _window_manager_set_fps(m);
  sprintf_s(buffer, "FPS: %.0f", m->fps);
  glbmfont_print(buffer, 5, 0, dock_top_right);
}

void window_manager_swapBuffers (window_manager *m)
{
  m->restart = !(!glfwGetKey( GLFW_KEY_ENTER ) && glfwGetWindowParam( GLFW_OPENED ));

  if(m->restart == 1) {
    m->running = 0;
    return;
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
  glDeleteLists(m->axes_cmd, 1);
  glfwTerminate();
  glbmfont_free();
}

static void _debug_init(window_manager *m)
{
  m->axes_cmd = glGenLists(1);
  glNewList(m->axes_cmd, GL_COMPILE);
  {
    glColor3f(.4f, .4f, .4f);
    glBegin(GL_LINES); glVertex2f(-1.f, .5f);   glVertex2f(1.f, .5f);   glEnd();
    glBegin(GL_LINES); glVertex2f(-1.f, -.5f);  glVertex2f(1.f, -.5f);  glEnd();
    glBegin(GL_LINES); glVertex2f(-.5f, 1.f);   glVertex2f(-.5f, -1.f); glEnd();
    glBegin(GL_LINES); glVertex2f(.5f, 1.f);    glVertex2f(.5f, -1.f);  glEnd();
    glBegin(GL_LINES); glVertex2f(-1.f, 0.f);   glVertex2f(0.f, 0.f);   glEnd();
    glBegin(GL_LINES); glVertex2f(0.f, 0.f);    glVertex2f(0.f,-10.f);  glEnd();

    
    glColor3f(.2f, .2f, .2f);
    glBegin(GL_LINES); glVertex2f(-.75f, 1.f);  glVertex2f(-.75f, -1.f);  glEnd();
    glBegin(GL_LINES); glVertex2f(-.25f, 1.f);  glVertex2f(-.25f, -1.f);  glEnd();
    glBegin(GL_LINES); glVertex2f(.25f, 1.f);   glVertex2f(.25f, -1.f);   glEnd();
    glBegin(GL_LINES); glVertex2f(.75f, 1.f);   glVertex2f(.75f, -1.f);   glEnd();
    glBegin(GL_LINES); glVertex2f(1.f, -.75f);  glVertex2f(-1.f, -.75f);  glEnd();
    glBegin(GL_LINES); glVertex2f(1.f, -.25f);  glVertex2f(-1.f, -.25f);  glEnd();
    glBegin(GL_LINES); glVertex2f(1.f, .25f);   glVertex2f(-1.f, .25f);   glEnd();
    glBegin(GL_LINES); glVertex2f(1.f, .75f);   glVertex2f(-1.f, .75f);   glEnd();

    glColor3f(0.f, 1.f, 0.f);
    glBegin(GL_LINES); glVertex2f(0.f, 1.f);    glVertex2f(0.f, 0.f);   glEnd();
    glColor3f(1.f, 0.f, 0.f);
    glBegin(GL_LINES); glVertex2f(0.f, 0.f);    glVertex2f(1.f, 0.f);   glEnd();
  } glEndList();
}
