#include "window_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

#include "glbmfont.h"

void _window_manager_gl_init(int height, int width);
void _window_manager_set_fps();

double current_fps = 0.0;

void _window_manager_set_fps (void)
{
	// Static values which only get initialised the first time the function runs
	static double t0Value       = glfwGetTime(); // Set the initial time to now
	static int    fpsFrameCount = 0;             // Set the initial FPS frame count to 0
 
	// Get the current time in seconds since the program started (non-static, so executed every time)
	double currentTime = glfwGetTime();
 
	// Calculate and display the FPS every specified time interval
	if ((currentTime - t0Value) > 0.1)
	{
		// Calculate the FPS as the number of frames divided by the interval in seconds
		current_fps = (double)fpsFrameCount / (currentTime - t0Value);
    
		// Reset the FPS frame counter and set the initial time to be now
		fpsFrameCount = 0;
		t0Value = glfwGetTime();
	}
	else // FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
	{
		fpsFrameCount++;
	}
}

void _window_manager_gl_init (int height, int width)
{
  glViewport(0, 0, width, height);

  glClearColor( 0.21f, 0.21f, 0.21f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT );
  glClear( GL_DEPTH_BUFFER_BIT );
}


void window_manager_init (window_manager * manager, char * title, int height, int width)
{
  manager->running = GL_TRUE;
  manager->restart = 0;
 
  // Initialize GLFW
  if( !glfwInit() )
  {
    exit( EXIT_FAILURE );
  }
  // Open an OpenGL window
  if( !glfwOpenWindow( width, height, 0,0,0,0,0,0, GLFW_WINDOW ) )
  {
    glfwTerminate();
    exit( EXIT_FAILURE );
  }
  //init de la fenêtre glfw
  glfwSetWindowTitle(title);
  
  _window_manager_gl_init(height, width);

  glbmfont_load();
}

void window_manager_clear (void)
{
  _window_manager_set_fps();
  
  glClear( GL_COLOR_BUFFER_BIT );
  glClear( GL_DEPTH_BUFFER_BIT );
    
  glLoadIdentity();
}

void window_manager_swapBuffers (window_manager * manager)
{
  manager->restart = !(!glfwGetKey( GLFW_KEY_ENTER ) && glfwGetWindowParam( GLFW_OPENED ));

  if(manager->restart == 1) {
    manager->running = 0;
    return;
  }

  // Swap front and back rendering buffers
  glfwSwapBuffers();
  // Check if ESC key was pressed or window was closed
  manager->running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
}

void window_manager_free (window_manager * manager)
{
  glfwTerminate();
  glbmfont_free();
}

double window_manager_getFps (void)
{
 return current_fps;
}
