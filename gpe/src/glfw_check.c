#include "glfw_check.h"

#include <stdio.h>
#include <stdlib.h>
#include <GL/glfw3.h>

void Init(void);
void Main_Loop(void);
void Draw_Square(float red, float green, float blue);
void Draw(void);
static void error_callback(int error, const char* description);

float rotate_y = 0,
      rotate_z = 0;

const float rotations_per_tick = 0.2f;

static GLFWwindow *_window;

void glfw_check_foo(void)
{
  Init();
  Main_Loop();

  glfwDestroyWindow(_window);
  _window = NULL;

  glfwTerminate();
}

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void Init(void)
{
  const int window_width = 800,
            window_height = 600;
  float aspect_ratio;
 
  glfwSetErrorCallback(error_callback);

  if (glfwInit() != GL_TRUE)
    exit(EXIT_FAILURE);

  _window = glfwCreateWindow(window_width, window_height, "The GLFW Window", NULL, NULL);
  if (!_window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(_window);

  // set the projection matrix to a normal frustum with a max depth of 50
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  aspect_ratio = ((float)window_height) / window_width;
  glFrustum(.5, -.5, -.5 * aspect_ratio, .5 * aspect_ratio, 1, 50);
  glMatrixMode(GL_MODELVIEW);
}
 
void Main_Loop(void)
{
  // the time of the previous frame
  double old_time = glfwGetTime();
  // this just loops as long as the program runs
  while(!glfwWindowShouldClose(_window))
  {
    // calculate time elapsed, and the amount by which stuff rotates
    double current_time = glfwGetTime(),
           delta_rotate = (current_time - old_time) * rotations_per_tick * 360;
    old_time = current_time;
    // escape to quit, arrow keys to rotate view
    if (glfwGetKey(_window, GLFW_KEY_ESC) == GLFW_PRESS)
      break;
    if (glfwGetKey(_window, GLFW_KEY_LEFT)== GLFW_PRESS)
      rotate_y = (float)(rotate_y + delta_rotate);
    if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      rotate_y = (float)(rotate_y - delta_rotate);
    // z axis always rotates
    rotate_z = (float)(rotate_z + delta_rotate);
 
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw the figure
    Draw();
    // swap back and front buffers
    glfwSwapBuffers(_window);
    glfwPollEvents();
  }
}
 
void Draw_Square(float red, float green, float blue)
{
  // Draws a square with a gradient color at coordinates 0, 10
  glBegin(GL_QUADS);
  {
    glColor3f(red, green, blue);
    glVertex2i(1, 11);
    glColor3f(red * .8f, green * .8f, blue * .8f);
    glVertex2i(-1, 11);
    glColor3f(red * .5f, green * .5f, blue * .5f);
    glVertex2i(-1, 9);
    glColor3f(red * .8f, green * .8f, blue * .8f);
    glVertex2i(1, 9);
  }
  glEnd();
}
 
void Draw(void)
{
  int i = 0, squares = 15;
  float red = 0, blue = 1;
  // reset view matrix
  glLoadIdentity();
  // move view back a bit
  glTranslatef(0, 0, -30);
  // apply the current rotation
  glRotatef(rotate_y, 0, 1, 0);
  glRotatef(rotate_z, 0, 0, 1);
  // by repeatedly rotating the view matrix during drawing, the
  // squares end up in a circle
  for (; i < squares; ++i){
    glRotatef((float)(360.0/squares), 0.0f, 0.0f, 1.0f);
    // colors change for each square
    red += (float)(1.0/12);
    blue -= (float)(1.0/12);
    Draw_Square(red, 0.6f, blue);
  }
}