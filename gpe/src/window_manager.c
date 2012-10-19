#include "window_manager.h"
#include <stdlib.h>

void window_manager_init (window_manager * manager, char * title, int height, int width)
{
  manager->running = GL_TRUE;
 
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

  //initialize viewing values 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glOrtho(0.0, 0.0, 0.0, 0.0, -1.0, 1.0);
}

void window_manager_clear (void)
{
  glClearColor( 0.21f, 0.21f, 0.21f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT );
}

void window_manager_swapBuffers (window_manager * manager)
{
  // Swap front and back rendering buffers
  glfwSwapBuffers();
  // Check if ESC key was pressed or window was closed
  manager->running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
}

void window_manager_free(window_manager * manager)
{
  glfwTerminate();
}

static const GLfloat circleVAR[] = {
	 0.0000f,  1.0000f,
	 0.2588f,  0.9659f,
	 0.5000f,  0.8660f,
	 0.7071f,  0.7071f,
	 0.8660f,  0.5000f,
	 0.9659f,  0.2588f,
	 1.0000f,  0.0000f,
	 0.9659f, -0.2588f,
	 0.8660f, -0.5000f,
	 0.7071f, -0.7071f,
	 0.5000f, -0.8660f,
	 0.2588f, -0.9659f,
	 0.0000f, -1.0000f,
	-0.2588f, -0.9659f,
	-0.5000f, -0.8660f,
	-0.7071f, -0.7071f,
	-0.8660f, -0.5000f,
	-0.9659f, -0.2588f,
	-1.0000f, -0.0000f,
	-0.9659f,  0.2588f,
	-0.8660f,  0.5000f,
	-0.7071f,  0.7071f,
	-0.5000f,  0.8660f,
	-0.2588f,  0.9659f,
	 0.0000f,  1.0000f,
	 0.0f, 0.0f, // For an extra line to see the rotation.
};

static const int circleVAR_count = sizeof(circleVAR)/sizeof(GLfloat)/2;

static inline void glColor_from_color(Color color){
	glColor4fv((GLfloat *)&color);
}

void drawCircle(cpVect center, cpFloat angle, cpFloat radius, Color lineColor, Color fillColor)
{
	glVertexPointer(2, GL_FLOAT, 0, circleVAR);

	glPushMatrix(); {
		glTranslatef(center.x, center.y, 0.0f);
		glRotatef(angle*180.0f/M_PI, 0.0f, 0.0f, 1.0f);
		glScalef(radius, radius, 1.0f);
		
		if(fillColor.a > 0){
			glColor_from_color(fillColor);
			glDrawArrays(GL_TRIANGLE_FAN, 0, circleVAR_count - 1);
		}
		
		if(lineColor.a > 0){
			glColor_from_color(lineColor);
			glDrawArrays(GL_LINE_STRIP, 0, circleVAR_count);
		}
	} glPopMatrix();
}

static const GLfloat pillVAR[] = {
	 0.0000f,  1.0000f, 1.0f,
	 0.2588f,  0.9659f, 1.0f,
	 0.5000f,  0.8660f, 1.0f,
	 0.7071f,  0.7071f, 1.0f,
	 0.8660f,  0.5000f, 1.0f,
	 0.9659f,  0.2588f, 1.0f,
	 1.0000f,  0.0000f, 1.0f,
	 0.9659f, -0.2588f, 1.0f,
	 0.8660f, -0.5000f, 1.0f,
	 0.7071f, -0.7071f, 1.0f,
	 0.5000f, -0.8660f, 1.0f,
	 0.2588f, -0.9659f, 1.0f,
	 0.0000f, -1.0000f, 1.0f,

	 0.0000f, -1.0000f, 0.0f,
	-0.2588f, -0.9659f, 0.0f,
	-0.5000f, -0.8660f, 0.0f,
	-0.7071f, -0.7071f, 0.0f,
	-0.8660f, -0.5000f, 0.0f,
	-0.9659f, -0.2588f, 0.0f,
	-1.0000f, -0.0000f, 0.0f,
	-0.9659f,  0.2588f, 0.0f,
	-0.8660f,  0.5000f, 0.0f,
	-0.7071f,  0.7071f, 0.0f,
	-0.5000f,  0.8660f, 0.0f,
	-0.2588f,  0.9659f, 0.0f,
	 0.0000f,  1.0000f, 0.0f,
};

static const int pillVAR_count = sizeof(pillVAR)/sizeof(GLfloat)/3;

void drawFatSegment(cpVect a, cpVect b, cpFloat radius, Color lineColor, Color fillColor)
{
	if(radius){
		glVertexPointer(3, GL_FLOAT, 0, pillVAR);
		glPushMatrix(); {
			cpVect d = cpvsub(b, a);
			cpVect r = cpvmult(d, radius/cpvlength(d));

			const GLfloat matrix[] = {
				 r.x, r.y, 0.0f, 0.0f,
				-r.y, r.x, 0.0f, 0.0f,
				 d.x, d.y, 0.0f, 0.0f,
				 a.x, a.y, 0.0f, 1.0f,
			};
			glMultMatrixf(matrix);
			
			if(fillColor.a > 0){
				glColor_from_color(fillColor);
				glDrawArrays(GL_TRIANGLE_FAN, 0, pillVAR_count);
			}
			
			if(lineColor.a > 0){
				glColor_from_color(lineColor);
				glDrawArrays(GL_LINE_LOOP, 0, pillVAR_count);
			}
		} glPopMatrix();
	} else {
		drawSegment(a, b, lineColor);
	}
}

void drawSegment(cpVect a, cpVect b, Color color)
{
	GLfloat verts[] = {
		a.x, a.y,
		b.x, b.y,
	};
	
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColor_from_color(color);
	glDrawArrays(GL_LINES, 0, 2);
}

void drawPolygon(int count, cpVect *verts, Color lineColor, Color fillColor)
{
#if CP_USE_DOUBLES
	glVertexPointer(2, GL_DOUBLE, 0, verts);
#else
	glVertexPointer(2, GL_FLOAT, 0, verts);
#endif
	
	if(fillColor.a > 0){
		glColor_from_color(fillColor);
		glDrawArrays(GL_TRIANGLE_FAN, 0, count);
	}
	
	if(lineColor.a > 0){
		glColor_from_color(lineColor);
		glDrawArrays(GL_LINE_LOOP, 0, count);
	}
}