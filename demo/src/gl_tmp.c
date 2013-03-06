#include "gl_tmp.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include "SOIL\SOIL.h"
#include "window_manager.h"
#include "GL\glfw.h"

#include <time.h>
#include "SOIL\SOIL.h"
#include "FreeImage\FreeImage.h"

#define HEIGHT      600
#define WIDTH       800
#define TEX_HEIGHT  128
#define TEX_WIDTH   128

void _init_sample_0();
void _init_sample_1();
void _init_sample_2();
void _init_sample_3();
void _display_sample_0(F32 r, F32 g, F32 b, F32 x, F32 y);
void _display_sample_1(F32 r, F32 g, F32 b, F32 x, F32 y);
void _display_sample_2(F32 r, F32 g, F32 b, F32 x, F32 y);
void _display_sample_3(F32 r, F32 g, F32 b, F32 x, F32 y);
static void _clean();
void _print_gl_error();
int RenderFrame();
void snapshot();

typedef struct
{
  U32 vbo[3];
  U32 vao;
} sample;

typedef struct
{
  I32     text_id;
  double  t0Value;
  int     fpsFrameCount;
  I32     fps;
  I32     last_fps;
  F32     last_gen;
  gpe_text_align align;
  gpe_text_dock  dock;
} frame_util;

static U32 tex_id;
static F32 verticies[] = { 0,0, 0,128, 128,128, 128,0 };
static F32 tex_coord[] = { 0,0, 0,1, 1,1, 1,0, };
static GLubyte indicies[] = { 0,1,2, 2,3,0 };

static sample sample_0; //VBO : glDrawArrays sur GL_QUADS
static sample sample_1; //VBO : glDrawElements sur GL_TRIANGLES avec buffer d'indice
static sample sample_2; //VAO : glDrawArrays sur GL_QUADS
static sample sample_3; //VAO : glDrawElements sur GL_TRIANGLES avec buffer d'indice

static F32 FRAME_DURATION = ((F32)1000/60)/1000;
static const F32 ANGLE_UNIT = M_PI/12;
static F32 angle = 0.f;
static frame_util _frame_util;

void GL_TMP_H_FOO()
{
  window_manager w;
  F32 ax, ay;

  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "gl_tmp.c : simple gl rendering test", HEIGHT, WIDTH);
  w.display_axes = 1;
  w.display_fps = 0;
  
  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS );

  _init_sample_0();
  _init_sample_1();
  _init_sample_2();
  _init_sample_3();

  _frame_util.text_id = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  _frame_util.last_gen = 0;  
  _frame_util.t0Value = glfwGetTime();
  _frame_util.fpsFrameCount = 0;
  _frame_util.fps = 0;
  _frame_util.align = ALIGN_TEXT_RIGHT;
  _frame_util.dock = DOCK_TEXT_TOP_RIGHT;
  font_system_text_set( _frame_util.text_id, L"FPS: 0", _frame_util.align );

  while(w.running)
  {
    if(!RenderFrame()) continue;

    window_manager_clear(&w);
    
    ax = 60*cos(angle);
    ay = 60*sin(angle);
          
    _display_sample_0(1.f, 0.f, 0.f, (WIDTH/4)-64+ax, (HEIGHT/4)-64+ay);
    _display_sample_1(0.f, 1.f, 0.f, (WIDTH/4)-64-ax, (3*HEIGHT/4)-64-ay);
    _display_sample_2(0.f, 0.f, 1.f, (3*WIDTH/4)-64-ax, (HEIGHT/4)-64-ay);
    _display_sample_3(1.f, 1.f, 0.f, (3*WIDTH/4)-64+ax, (3*HEIGHT/4)-64+ay);
    _display_sample_3(1.f, 1.f, 1.f, (WIDTH-128)/2, (HEIGHT-128)/2);
    
    angle += ANGLE_UNIT;
 
    font_system_text_print( _frame_util.text_id, 6, 0, _frame_util.dock, HEIGHT, WIDTH);

    window_manager_swapBuffers(&w);

    snapshot();
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
  glEnableClientState(GL_VERTEX_ARRAY);


  glBindBuffer(GL_ARRAY_BUFFER, sample_2.vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

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
  glVertexPointer( 2, GL_FLOAT, 0, 0 );
  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sample_3.vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, sample_3.vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
}


void _display_sample_0(F32 r, F32 g, F32 b, F32 x, F32 y)
{
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glPushMatrix();
  {
    glTranslatef(x, y, 0.f);
    glColor3f(r, g, b);
 
    glBindBuffer(GL_ARRAY_BUFFER, sample_0.vbo[0]);
    glVertexPointer( 2, GL_FLOAT, 0, 0 );
    glBindBuffer(GL_ARRAY_BUFFER, sample_0.vbo[1]);
    glTexCoordPointer( 2, GL_FLOAT, 0, 0 );

    glDrawArrays(GL_QUADS, 0, 4);
    glBindBuffer(GL_ARRAY_BUFFER, 0 );

  } glPopMatrix();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void _display_sample_1(F32 r, F32 g, F32 b, F32 x, F32 y)
{
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glPushMatrix();
  {
    glTranslatef(x, y, 0.f);
    glColor3f(r, g, b);
 
    glBindBuffer( GL_ARRAY_BUFFER, sample_1.vbo[0] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, sample_1.vbo[2] );
    glVertexPointer( 2, GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, sample_1.vbo[1] );
    glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
    
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0 );

  } glPopMatrix();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void _display_sample_2(F32 r, F32 g, F32 b, F32 x, F32 y)
{
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glPushMatrix();
  {
    glTranslatef(x, y, 0.f);
    glColor3f(r, g, b);
 
    glBindVertexArray(sample_2.vao);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);

  } glPopMatrix();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void _display_sample_3(F32 r, F32 g, F32 b, F32 x, F32 y)
{
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glPushMatrix();
  {
    glTranslatef(x, y, 0.f);
    glColor3f(r, g, b);
 
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

int RenderFrame()
{
  int render = 0;
	double currentTime = glfwGetTime();
	if ((currentTime - _frame_util.last_gen) >= FRAME_DURATION)
  {
    _frame_util.last_gen = currentTime;
    render = 1;
  }

  if ((currentTime -  _frame_util.t0Value) >= 1.0)
	{
		 _frame_util.fps = (I32)( _frame_util.fpsFrameCount / (currentTime -  _frame_util.t0Value));
		 _frame_util.fpsFrameCount = 0;
		 _frame_util.t0Value = currentTime;

    if( _frame_util.last_fps !=  _frame_util.fps)
    {
      wchar_t fps[128];
      _frame_util.last_fps =  _frame_util.fps;
      swprintf(fps, 128, L"FPS: %d",   _frame_util.fps);
      font_system_text_set( _frame_util.text_id, fps, _frame_util.align);
    }
	}
	else if(render == 1)
		 _frame_util.fpsFrameCount++;

  return render;
}


void snapshot()
{
  if(glfwGetKey( GLFW_KEY_DEL))
  {
    clock_t start = clock();
    //JPEG
    FIBITMAP *image;
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; 
    BYTE *pixels = (BYTE*)malloc(sizeof(BYTE)*3 * WIDTH * HEIGHT);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, pixels);
    image = FreeImage_ConvertFromRawBits(pixels, WIDTH, HEIGHT, 3*WIDTH, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, 0);
    FreeImage_Save(FIF_JPEG, image, "e:\\tmp.jpg", JPEG_QUALITYSUPERB);
    free(pixels);
    printf("JPEG: %.5f\n", (float)( clock () - start )/CLOCKS_PER_SEC);
    //PNG
    start = clock();
    SOIL_save_screenshot("e:\\tmp.png", SOIL_SAVE_TYPE_BMP, 0, 0, WIDTH, HEIGHT);
    printf("PNG:  %.5f\n", (float)( clock () - start )/CLOCKS_PER_SEC);
  }
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