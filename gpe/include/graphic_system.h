#ifndef graphic_system_h
#define graphic_system_h

#include "gpr_types.h"
#include "gpr_idlut.h"
#include "gpr_array.h"

#include "gl_renderer.h"

typedef struct
{
  GLuint tex_id;
  F32    x, y, w, h;
  F32    scale;
  F32    r, a;
  F32    shear_x, shear_y;
  I32    z;
} gpe_graphic_t;


typedef struct
{
  gpr_idlut_t     table;
  gpe_gl_renderer renderer;
} graphic_system;


void            graphic_system_init   (graphic_system *system);
U64             graphic_system_add    (graphic_system *system, GLuint tex_id);
U64             graphic_system_add    (graphic_system *system, gpe_graphic_t graphic);
gpe_graphic_t*  graphic_system_lookup (graphic_system *system, U64 graphic_id);
void            graphic_system_remove (graphic_system *system, U64 graphic_id);
void            graphic_system_update (graphic_system *system);
void            graphic_system_render (graphic_system *system);
void            graphic_system_free   (graphic_system *system);

#endif //graphic_system_h