#include "gl_tmp.h"

#include <stdio.h>
#include <SOIL\SOIL.h>
#include "window_manager.h"

#define HEIGHT  600
#define WIDTH   800

void _init_sample_0();
void _init_sample_1();
void _init_sample_2();
void _init_sample_3();
void _display_sample_0();
void _display_sample_1();
void _display_sample_2();
void _display_sample_3();
static void _clean();
void _print_gl_error();

typedef struct
{
  U32 vbo[3];
  U32 vao;
} sample;

static U32 tex_id;
static F32 verticies[] = { 0,0, 0,128, 128,128, 128,0 };
static F32 tex_coord[] = { 0,0, 0,1, 1,1, 1,0, };
static GLubyte indicies[] = { 0,1,2, 2,3,0 };

static sample sample_0; //VBO : glDrawArrays sur GL_QUADS
static sample sample_1; //VBO : glDrawElements sur GL_TRIANGLES
static sample sample_2; //VAO : glDrawArrays sur GL_QUADS
static sample sample_3; //VAO : glDrawElements sur GL_TRIANGLES

void GL_TMP_H_FOO()
{
  window_manager w;

  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "gl_tmp.c : simple gl rendering test", HEIGHT, WIDTH);
  w.display_axes = 1;
  w.display_fps = 1;

  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS );
    
  _init_sample_0();
  _init_sample_1();
  _init_sample_2();
  _init_sample_3();

  while(w.running)
  {
    window_manager_clear(&w);

    _display_sample_0();
    _display_sample_1();
    _display_sample_2();
    _display_sample_3();
    
    window_manager_swapBuffers(&w);
  }

  _clean();

  window_manager_free(&w);
  gpr_memory_shutdown();
}

void _init_sample_0()
{
  glGenBuffers(2, sample_0.vbo);

  glBindBuffer(GL_ARRAY_BUFFER, sample_0.vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, sample_0.vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

void _init_sample_1()
{
  glGenBuffers(3, sample_1.vbo);
  
  glBindBuffer(GL_ARRAY_BUFFER, sample_1.vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, sample_1.vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sample_1.vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

void _init_sample_2()
{
  glGenVertexArrays(1, &sample_2.vao);

  glBindVertexArray(sample_2.vao);
  glGenBuffers(2, sample_2.vbo);

  glBindBuffer(GL_ARRAY_BUFFER, sample_2.vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  glVertexPointer( 2, GL_FLOAT, 0, 0 );
  /*
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  */

  glBindBuffer(GL_ARRAY_BUFFER, sample_2.vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
  /*
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  */

  glBindBuffer(GL_ARRAY_BUFFER, 0 );
  glBindVertexArray(0);
}

void _init_sample_3()
{
  glGenVertexArrays(1, &sample_3.vao);

  glBindVertexArray(sample_3.vao);
  glGenBuffers(3, sample_3.vbo);

  glBindBuffer(GL_ARRAY_BUFFER, sample_3.vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sample_3.vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
  glVertexPointer( 2, GL_FLOAT, 0, 0 );
  /*
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
  */

  glBindBuffer(GL_ARRAY_BUFFER, sample_3.vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
  /*
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
  */

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
}


void _display_sample_0()
{
  glPushMatrix();
  {
    glTranslatef(0.f, 0.f, 0.f);
    glColor3f(1.f, 1.f, 1.f);
 
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glBindBuffer(GL_ARRAY_BUFFER, sample_0.vbo[0]);
    glVertexPointer( 2, GL_FLOAT, 0, 0 );
    glBindBuffer(GL_ARRAY_BUFFER, sample_0.vbo[1]);
    glTexCoordPointer( 2, GL_FLOAT, 0, 0 );

    glDrawArrays(GL_QUADS, 0, 4);
    glBindBuffer(GL_ARRAY_BUFFER, 0 );

  } glPopMatrix();
  glBindTexture(GL_TEXTURE_2D, 0);
}

void _display_sample_1()
{
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glPushMatrix();
  {
    glTranslatef(0.f, HEIGHT-128, 0.f);
    glColor3f(1.f, 1.f, 1.f);
 
    glBindBuffer( GL_ARRAY_BUFFER, sample_1.vbo[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, sample_1.vbo[2] );
    glVertexPointer( 2, GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, sample_1.vbo[1] );
    glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0 );

    glBindTexture(GL_TEXTURE_2D, 0);
  } glPopMatrix();
}

void _display_sample_2()
{
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glPushMatrix();
  {
    glTranslatef(WIDTH-128, 0.f, 0.f);
    glColor3f(1.f, 1.f, 1.f);
 
    glBindVertexArray(sample_2.vao);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);

  } glPopMatrix();
  glBindTexture(GL_TEXTURE_2D, 0);
}

void _display_sample_3()
{
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glPushMatrix();
  {
    glTranslatef(WIDTH-128, HEIGHT-128, 0.f);
    glColor3f(1.f, 1.f, 1.f);
 
    glBindVertexArray(sample_3.vao);
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    glBindVertexArray(0);

  } glPopMatrix();
  glBindTexture(GL_TEXTURE_2D, 0);
}


static void _clean()
{
  glDeleteBuffers(2, sample_0.vbo);

  glDeleteBuffers(3, sample_1.vbo);
  
  glDeleteVertexArrays(1, &sample_2.vao);
  glDeleteBuffers(2, sample_2.vbo);
  
  glDeleteVertexArrays(1, &sample_3.vao);
  glDeleteBuffers(3, sample_3.vbo);

  glDeleteTextures(1, &tex_id);
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