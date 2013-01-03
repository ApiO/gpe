#ifndef graphic_system_h
#define graphic_system_h

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>

#include <gpr_types.h>

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

U32   graphic_system_add  (gpe_graphic *gData, U32 tex_id);
void  graphic_remove  (gpe_graphic *gData, U32 tex_id);
void  graphic_system_render (gpe_graphic *gData);

#endif //graphic_system_h