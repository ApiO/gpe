#ifndef graphic_system_h
#define graphic_system_h

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>

#include "gpr_types.h"
#include "gpr_idlut.h"

const int TEX_TOP_LEFT      = 0;
const int TEX_BOTTOM_LEFT   = 1;
const int TEX_BOTTOM_RIGHT  = 2;
const int TEX_TOP_RIGHT     = 3;

typedef struct FVect
{
  GLfloat x, y;
} FVect;

typedef struct gpe_graphic
{
  GLuint  text_id;
  FVect   texCoord[4];
  F32     x, y, w, h;
  F32     scale;
  F32     r, a;
  F32     shear_x, shear_y;
  I32     z;
} gpe_graphic;
GPR_IDLUT_INIT(gpe_graphic);

typedef struct graphic_system
{
  gpr_idlut_t(gpe_graphic) table;
  U32   physics_count; 
  U32   capacity;
} graphic_system;

void          graphic_system_init   (graphic_system *system, U32 object_count);
U32           graphic_system_add    (graphic_system *system, GLuint tex_id);
gpe_graphic*  graphic_system_lookup (graphic_system *system, U32 graphic_id);
void          graphic_system_remove (graphic_system *system, U32 graphic_id);
void          graphic_system_render (graphic_system *system);
void          graphic_system_free   (graphic_system *system);

#endif //graphic_system_h