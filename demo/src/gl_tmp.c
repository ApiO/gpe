#include "libwebsockets/lib/libwebsockets.h"
#include "gl_tmp.h"

#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include "SOIL\SOIL.h"
#include "window_manager.h"
#include "GL\glfw3.h"

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
void snapshot(unsigned char *buf, int *buf_size);

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

typedef struct
{
  unsigned char *byte_buf;
  char *b64_buf;
  I32 send;
} user_data_t;

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


static void base64_encode(const unsigned char *data,
                          int input_length,
                          char *out,
                          int *output_length)
{
  *output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));

  for (int i = 0, j = 0; i < input_length;) {

    uint32_t octet_a = i < input_length ? data[i++] : 0;
    uint32_t octet_b = i < input_length ? data[i++] : 0;
    uint32_t octet_c = i < input_length ? data[i++] : 0;

    uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    out[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
    out[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
    out[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
    out[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (int i = 0; i < mod_table[input_length % 3]; i++)
    out[*output_length - 1 - i] = '=';
}

static int callback_engine(struct libwebsocket_context *context,
struct libwebsocket *wsi,
  enum libwebsocket_callback_reasons reason, void *user,
  void *in, size_t len)
{
  user_data_t *pss = (user_data_t*)user;
  switch(reason)
  {
  case LWS_CALLBACK_ESTABLISHED:
    printf("connection established\n");
    pss->byte_buf = (unsigned char*)malloc(3*800*600);
    pss->b64_buf  = (char*)malloc(LWS_SEND_BUFFER_PRE_PADDING + 3*800*600 + LWS_SEND_BUFFER_POST_PADDING);
    break;
  case LWS_CALLBACK_PROTOCOL_DESTROY:
    free(pss->byte_buf);
    free(pss->b64_buf);
    printf("connection closed\n");
  case LWS_CALLBACK_SERVER_WRITEABLE:
    {
      /*if (!pss->send)
      {
        pss->send = 1;
        return 0;
      }*/

      int n, size;

      FIBITMAP *img;
      BYTE *pixels;
      long file_size;
      int save, acquire;
      FIMEMORY *hmem;
      char *b64;

      pixels = (BYTE*)malloc(sizeof(BYTE)*3 * WIDTH * HEIGHT);
      hmem = FreeImage_OpenMemory();
      glReadPixels(0, 0, WIDTH, HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, pixels);
      img = FreeImage_ConvertFromRawBits(pixels, WIDTH, HEIGHT, 3*WIDTH, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, 0);
      save = FreeImage_SaveToMemory(FIF_JPEG, img, hmem, JPEG_QUALITYSUPERB);
      FreeImage_Unload(img);
      free(pixels);

      // Get the buffer from the memory stream.
      acquire = FreeImage_AcquireMemory(hmem, &pss->byte_buf, (DWORD*)&n);
      b64 = pss->b64_buf + LWS_SEND_BUFFER_PRE_PADDING;
      base64_encode(pss->byte_buf, n, b64, &size);

      n = libwebsocket_write(wsi, (unsigned char*)b64, size, LWS_WRITE_TEXT);
      FreeImage_CloseMemory(hmem);

      pss->send = 0;
    }
    break;
  case LWS_CALLBACK_RECEIVE:
    printf("message recieved\n");
    break;
  default : 
    break;
  }
  return 0;
}

static struct libwebsocket_protocols protocols[] = {
  // first protocol must always be HTTP handler
  {
    "engine-protocol",    // name
      callback_engine,    // callback
      sizeof(user_data_t) // per_session_data_size
  },
  {
    NULL, NULL, 0       // end of list
    }
};

void GL_TMP_H_FOO()
{
  window_manager w;
  F32 ax, ay;

  struct libwebsocket_context *context;
  struct lws_context_creation_info info;

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

  // create libwebsocket context representing this server
  memset(&info, 0, sizeof info);
  info.port = 9000;
  info.protocols = protocols;

  context = libwebsocket_create_context(&info);

  if (context == NULL) {
    fprintf(stderr, "libwebsocket init failed\n");
    return;
  }

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

    //snapshot();
    libwebsocket_callback_on_writable_all_protocol(&protocols[0]);
    libwebsocket_service(context, 0);
  }

  libwebsocket_context_destroy(context);

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