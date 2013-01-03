#ifndef graphic_system_h
#define graphic_system_h

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>

#include "gpr_types.h"
#include "gpr_containers.h"

typedef struct gpe_graphic
{
  GLuint text_id;
  F32 x, y;
  F32 scale;
  F32 tex_w, tex_h;
  F32 r, a;
  F32 shear_x, shear_y;
  I32 z;
} gpe_graphic;
GPR_IDLUT_INIT(gpe_graphic);

typedef struct graphic_manager
{
  gpr_idlut_t(gpe_graphic) table;
  U32 physics_count; 
} graphic_manager;

void  graphic_system_init     (graphic_manager *system);
U32   graphic_system_add      (graphic_manager *system, U32 tex_id);
void  graphic_system_remove   (graphic_manager *system, U32 graphic_id);
void  graphic_system_render   (graphic_manager *system);
void  graphic_system_free     (graphic_manager *system);

#endif //graphic_system_h