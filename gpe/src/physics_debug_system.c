#include "physics_debug_system.h"

#include <GL\glfw.h>
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

void physics_debug_system_draw (physics_debug_system * system)
{
	int width, height;
	glfwGetWindowSize(&width, &height);
	reshape(width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(scale, scale, 1.0);
	glTranslatef(translate_x, translate_y, 0.0);

	ChipmunkDebugDrawShapes(system->space);
	ChipmunkDebugDrawConstraints(system->space);
	ChipmunkDebugDrawCollisionPoints(system->space);
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	
	double scale = cpfmin(width/640.0, height/480.0);
	double hw = width*(0.5/scale);
	double hh = height*(0.5/scale);
	
	ChipmunkDebugDraw_SetPointLineScale(scale);
	glLineWidth(scale);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-hw, hw, -hh, hh, -1.0, 1.0);
	glTranslated(0.5, 0.5, 0.0);
}