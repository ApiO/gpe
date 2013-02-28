#include "gl_tmp.h"

#include <stdio.h>
#include <SOIL\SOIL.h>
#include "window_manager.h"

#define HEIGHT  600
#define WIDTH   800

void _print_gl_error();

void GL_TMP_H_FOO()
{
  window_manager w;
  U32 vbo[3];
  U32 vao;
  U32 tex_id;
  F32 verticies[] = { 0,0, 0,128, 128,128, 128,0 };
  F32 tex_coord[] = { 0,0, 0,1, 1,1, 1,0, };
  GLubyte indicies[] = { 0,1,2, 2,3,0 };

  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "gl_tmp.c : simple gl rendering test", HEIGHT, WIDTH);
  w.display_axes = 1;
  w.display_fps = 1;

  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS );

      glBindTexture(GL_TEXTURE_2D, tex_id);
  
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  
  glGenBuffers(1, &vbo[2]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
      glVertexPointer( 2, GL_FLOAT, 0, 0 );
  
  glGenBuffers(1, &vbo[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
      glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0 );


  while(w.running)
  {
    window_manager_clear(&w);
          
   glPushMatrix();
    {
      glTranslatef(HEIGHT/2, HEIGHT/2, 0.f);
      glColor3f(1.f, 1.f, 1.f);
      glScalef(1.f, 1.f, 1.f);
 
      glBindVertexArray(vao);
      glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
      glBindVertexArray(0);
      //glBindBuffer(GL_ARRAY_BUFFER, 0 );

    } glPopMatrix();

    window_manager_swapBuffers(&w);
  }

  
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(3, vbo);
  glDeleteTextures(1, &tex_id);

  window_manager_free(&w);
  gpr_memory_shutdown();
}


void _print_gl_error()
{
  GLenum err = glGetError();
  if (err != GL_NO_ERROR)
  {
    char buffer[1024];
    sprintf_s(buffer, "\n----\nOpenGl error: %s\n\tline: %d\n\tfile: %s\n----\n", 
      glewGetErrorString(err), __LINE__, __FILE__);
    OutputDebugString(buffer);
    exit(EXIT_FAILURE); 
  }
}