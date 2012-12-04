#include "window_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

#include "fonts.h"

void gl_init(int height, int width);
void printFPS();

int SCREEN_W, SCREEN_H;

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
  
  gl_init(height, width);

  SCREEN_W = width;
  SCREEN_H = height;

  fonts_foo_load();
}

void gl_init(int height, int width)
{
  glViewport(0, 0, width, height);

  glClearColor( 0.21f, 0.21f, 0.21f, 0.0f );
  glClear(GL_COLOR_BUFFER_BIT);
	
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);

  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
	
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void window_manager_clear (void)
{
  glClear( GL_COLOR_BUFFER_BIT );

  // initialize viewing values 
  glMatrixMode(GL_PROJECTION);
  
  glLoadIdentity();

  printFPS();
  fonts_foo_draw();
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

void window_manager_free(window_manager * manager)
{
  glfwTerminate();
  fonts_foo_free();
}

void printFPS()
{
	// Static values which only get initialised the first time the function runs
	static double t0Value       = glfwGetTime(); // Set the initial time to now
	static int    fpsFrameCount = 0;             // Set the initial FPS frame count to 0
	double fps           = 0.0;           // Set the initial FPS value to 0.0
  char   msg[8];
 
	// Get the current time in seconds since the program started (non-static, so executed every time)
	double currentTime = glfwGetTime();
 
	// Calculate and display the FPS every specified time interval
	if ((currentTime - t0Value) > 0.1)
	{
		// Calculate the FPS as the number of frames divided by the interval in seconds
		fps = (double)fpsFrameCount / (currentTime - t0Value);
    sprintf_s(msg, "%.0f\n", fps);
    
    //printf_s(msg);

    fonts_bitmap(msg, 10, 10);

		// Reset the FPS frame counter and set the initial time to be now
		fpsFrameCount = 0;
		t0Value = glfwGetTime();
	}
	else // FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
	{
		fpsFrameCount++;
	}
}