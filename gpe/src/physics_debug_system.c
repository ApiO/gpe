#include "physics_debug_system.h"

#include "GL\glfw3.h"
#include "ChipmunkDebugDraw.h"

GLfloat translate_x = 0.0;
GLfloat translate_y = 0.0;
GLfloat scale = 1.0;

void reshape(int width, int height);

void physics_debug_system_init (physics_debug_system * system, cpSpace * space)
{
	system->space = space;
	ChipmunkDebugDraw_SetPointLineScale(1.0f);
}

void physics_debug_system_draw (int width, int height, physics_debug_system * system)
{      	
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	reshape(width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(scale, scale, 1.0);
	glTranslatef(translate_x, translate_y, 0.0);

	ChipmunkDebugDrawShapes(system->space);
	ChipmunkDebugDrawConstraints(system->space);
	ChipmunkDebugDrawCollisionPoints(system->space);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
}

void reshape(int width, int height)
{
	GLfloat scale = (GLfloat)cpfmin(width/640.0, height/480.0);
	double hw = width*(0.5/scale);
	double hh = height*(0.5/scale);

	glViewport(0, 0, width, height);
	
	ChipmunkDebugDraw_SetPointLineScale(scale);
	glLineWidth(scale);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-hw, hw, -hh, hh, -1.0, 1.0);
	glTranslated(0.5, 0.5, 0.0);
}