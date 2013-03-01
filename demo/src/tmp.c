#include "tmp.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GL\glfw.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <SOIL\SOIL.h>

#define NumberOf(arr) (sizeof(arr)/sizeof(arr[0]))

#define WIDTH  800
#define HEIGHT 600

typedef struct {
  GLfloat x, y, z;
  } vec3;

typedef struct {
  vec3    xlate;
  GLfloat angle;
  vec3    axis;
} XForm;

static GLuint tex_id;
static enum { Cube, Cone, NumVAOs };
static GLuint VAO[NumVAOs];
static GLenum PrimType[NumVAOs];
static GLsizei NumElements[NumVAOs];
static XForm Xform[NumVAOs] = {
  { { -2.0, 0.0, 0.0 }, 0.0, { 0.0, 1.0, 0.0 } },
  { { 0.0, 0.0, 2.0 }, 0.0, { 1.0, 0.0, 0.0 } }
};
static GLfloat Angle = 0.0;
static enum { Vertices, Colors, Elements, TexCoord, NumVBOs };
static GLuint buffers[NumVBOs];

static void _clean();
static void _display();
static void _init();
static void _init_gl();
static void _reshape();

void FOO()
{
  _init();
  
  do
  {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    _reshape();

    glColor3f(1.f, 1.f, 1.f);
    _display();
    
    glfwSwapBuffers();

  }while( !glfwGetKey( GLFW_KEY_ENTER ) && glfwGetWindowParam( GLFW_OPENED ));

  _clean();
}

static void _init_gl()
{
  GLenum err;
  if( !glfwInit() ) exit( EXIT_FAILURE );

  if( !glfwOpenWindow( 800, 600, 0,0,0,0,0,0, GLFW_WINDOW ) )
  {
    glfwTerminate();
    exit( EXIT_FAILURE );
  }
  glfwEnable( GLFW_STICKY_KEYS );

  err = glewInit();
  if (GLEW_OK != err) 
  { 
    char buffer[1024];
        sprintf(buffer, "\n----\nGlew init error: %s\n\tline: %d\n\tfile: %s\n----\n", 
      glewGetErrorString(err), __LINE__, __FILE__);
    OutputDebugString(buffer);
    exit(EXIT_FAILURE); 
  }

  glfwSetWindowTitle("TMP.c");
  
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor( .1f, .1f, .1f, 0.0f );
  
  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS );
}

#define cpfmin(a, b) ((a < b) ? a : b)

float pad_x = 0;
float pad_y = 0;
float pad_z = 0;
float speed = .5f;

static void _reshape()
{
  int width, height;
	GLfloat scale;
  glfwGetWindowSize(&width, &height);
  scale = (GLfloat)cpfmin(width/640.0, height/480.0);

	double hw = width*scale*0.005;
	double hh = height*scale*0.005;

	glViewport(0, 0, width, height);
	
	glLineWidth(scale);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
  if (glfwGetKey( GLFW_KEY_UP ) ==      GLFW_PRESS)  pad_y -= speed;
  if (glfwGetKey( GLFW_KEY_DOWN ) ==    GLFW_PRESS)  pad_y += speed;
  if (glfwGetKey( GLFW_KEY_RIGHT ) ==   GLFW_PRESS)  pad_x -= speed;
  if (glfwGetKey( GLFW_KEY_LEFT ) ==    GLFW_PRESS)  pad_x += speed;
  if (glfwGetKey( GLFW_KEY_PAGEUP ) ==  GLFW_PRESS)  pad_z -= speed;
  if (glfwGetKey( GLFW_KEY_PAGEDOWN) == GLFW_PRESS)  pad_z += speed;
  
  glOrtho(-hw+pad_x, hw+pad_x, -hh+pad_y, hh+pad_y, -1.0+pad_z, 1.0+pad_z);

	//glTranslated(0.5, 0.5, 0.0);
}


static void _init()
{
  _init_gl();

  glGenVertexArrays(NumVAOs, VAO);
  {
    GLfloat cubeVerts[][3] = {
        { -1.0, -1.0, -1.0 },
        { -1.0, -1.0, 1.0 },
        { -1.0, 1.0, -1.0 },
        { -1.0, 1.0, 1.0 },
        { 1.0, -1.0, -1.0 },
        { 1.0, -1.0, 1.0 },
        { 1.0, 1.0, -1.0 },
        { 1.0, 1.0, 1.0 },
      };
    GLfloat tex_coord[][2] = { {0.,0.}, {0.,1.}, {1.,1.}, {1.,0.}, };
    GLfloat cubeColors[][3] = {
        { 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 1.0, 1.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 0.0, 1.0 },
        { 1.0, 1.0, 0.0 },
        { 1.0, 1.0, 1.0 },
      };
    GLubyte cubeIndices[] = {
        0, 1, 3, 2,
        4, 6, 7, 5,
        2, 3, 7, 6,
        0, 4, 5, 1,
        0, 2, 6, 4,
        1, 5, 7, 3
      };

    glBindVertexArray(VAO[Cube]);
    glGenBuffers(NumVBOs, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[Colors]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);
    glColorPointer(3, GL_FLOAT, 0, 0);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[TexCoord]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
    glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    PrimType[Cube] = GL_QUADS;
    NumElements[Cube] = NumberOf(cubeIndices);
  }
  {
    int i, idx;
    float dTheta;
    #define NumConePoints 36
    // We add one more vertex for the cone's apex 
    GLfloat coneVerts[NumConePoints+1][3] = { {0.0, 0.0, 1.0} };
    GLfloat coneColors[NumConePoints+1][3] = { {1.0, 1.0, 1.0} };
    GLubyte coneIndices[NumConePoints+1];
    dTheta = 2*M_PI / (NumConePoints - 1);
    idx = 1;

    for (i = 0; i < NumConePoints; ++i, ++idx) {
      float theta = i*dTheta;
      coneVerts[idx][0] = cos(theta);
      coneVerts[idx][1] = sin(theta);
      coneVerts[idx][2] = 0.0;
      coneColors[idx][0] = cos(theta);
      coneColors[idx][1] = sin(theta);
      coneColors[idx][2] = 0.0;
      coneIndices[idx] = idx;
    }

    glBindVertexArray(VAO[Cone]);
    glGenBuffers(NumVBOs, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[Vertices]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coneVerts), coneVerts, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[Colors]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coneColors), coneColors, GL_STATIC_DRAW);
    glColorPointer(3, GL_FLOAT, 0, 0);
    glEnableClientState(GL_COLOR_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[Elements]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coneIndices), coneIndices, GL_STATIC_DRAW);

    PrimType[Cone] = GL_TRIANGLE_FAN;
    NumElements[Cone] = NumberOf(coneIndices);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

static void _display()
{
  int i;

  glPushMatrix();
  glRotatef(Angle, 0.0, 1.0, 0.0);


  for (i = 0; i < NumVAOs; ++i) 
  {
    if(i==0)
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex_id);
    }

    glPushMatrix();
    {
      glTranslatef(Xform[i].xlate.x, Xform[i].xlate.y, Xform[i].xlate.z);
      glRotatef(Xform[i].angle, Xform[i].axis.x, Xform[i].axis.y, Xform[i].axis.z);

      glBindVertexArray(VAO[i]);
      glDrawElements(PrimType[i], NumElements[i], GL_UNSIGNED_BYTE, 0);
      glBindVertexArray(0);

    } glPopMatrix();

    if(i==0)
    {
      glDisable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
  glPopMatrix();
}

static void _clean()
{
  glDeleteVertexArrays(NumVAOs, VAO);
  glDeleteBuffers(NumVBOs, buffers);
  
  glDeleteTextures(1, &tex_id);

  glfwTerminate();
}

