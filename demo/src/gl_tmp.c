#include "gl_tmp.h"

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>
#include <SOIL\SOIL.h>
#include <GL\glfw.h>

#include "gpr_types.h"

#include "window_manager.h"

#define HEIGHT 600
#define WIDTH  800

void draw_scene(GLuint tex_id);
void draw_sprite(GLuint tex_id);

GLint cam_x = 0;
GLint cam_y = 0;
I32 tex_height = 128;

void GL_TMP_H_FOO()
{
  U32 tex_id;
  window_manager window;
  F64 transform[4][4];

  window_manager_init(&window, "simple gl rendering test", HEIGHT, WIDTH);

  //window.display_fps = false;

  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_INVERT_Y );

  if(tex_id != 0)
  {
    //glEnableClientState(GL_VERTEX_ARRAY);
/*
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    glEnable(GL_TEXTURE_2D);
*/

    while(window.running)
    {
      
      cam_x += glfwGetKey( GLFW_KEY_LEFT ) ? 1 : 0;
      cam_x -= glfwGetKey( GLFW_KEY_RIGHT ) ? 1 : 0;
      cam_y -= glfwGetKey( GLFW_KEY_UP ) ? 1 : 0;
      cam_y += glfwGetKey( GLFW_KEY_DOWN ) ? 1 : 0;

      glViewport(cam_x, cam_y, WIDTH, HEIGHT);

      window_manager_clear(&window);

      draw_scene(tex_id);

      window_manager_swapBuffers(&window);
    }
 /*
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glBlendFunc(GL_NONE, GL_NONE);  
    //glDisableClientState(GL_VERTEX_ARRAY);
*/
    glDeleteTextures(1, &tex_id);
  }

  window_manager_clear(&window);
}

void draw_scene(GLuint tex_id)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  glPushMatrix();
  {
    glTranslatef(-(float)tex_height/2/WIDTH, -(float)tex_height/2/HEIGHT, .0f);			// Place at Center
	  glRotatef(.0f, .0f, .0f, .0f);
	  glScalef( (float)128/WIDTH, (float)128/HEIGHT, .0f);

	  draw_sprite(tex_id);

  } glPopMatrix();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
}

void draw_sprite(GLuint tex_id)
{
  glEnable(GL_TEXTURE_2D);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

  glBindTexture(GL_TEXTURE_2D, tex_id);

  glBegin(GL_QUADS);
  {
 	  glTexCoord2f( -1, 1 );
 	  glVertex2d( -1, 1 );
      
 	  glTexCoord2f( -1, -1 );
 	  glVertex2d( -1, -1 );
      
 	  glTexCoord2f( 1, -1 );
 	  glVertex2d( 1, -1 );
 
 	  glTexCoord2f( 1, 1 );
 	  glVertex2d( 1, 1 );
	} 
  glEnd();

  glDisable(GL_TEXTURE_2D);
}