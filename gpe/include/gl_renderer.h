#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>

#include "gpr_types.h"
#include "gpr_idlut.h"
#include "gpr_array.h"
/*
typedef struct
{
  GLfloat x, y;
} FVect;

typedef struct
{
  GLdouble x, y;
} DVect;
*/
typedef struct
{
  GLuint  tex_id;
  //FVect   texCoord[4];
  //DVect   quad[4];
  F32     x, y, w, h;
  F32     scale;
  F32     r, a;
  F32     shear_x, shear_y;
  I32     z;
} gpe_gl_graphic_t;

typedef struct 
{
  //gpr_idlut_t table;
} gpe_gl_renderer;

void gl_renderer_init   (gpe_gl_renderer *r);
void gl_renderer_add    (gpe_gl_renderer *r, gpe_gl_graphic_t graphic);
void gl_renderer_update (gpe_gl_renderer *r);
void gl_renderer_draw   (gpe_gl_renderer *r);
void gl_renderer_free   (gpe_gl_renderer *r);

#endif //GL_RENDERER_H