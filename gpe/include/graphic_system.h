#ifndef graphic_system_h
#define graphic_system_h

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>

#include <gpr_types.h>

typedef struct gpe_graphic
{
  F32 x, y;
  //GLuint text_id;
} gpe_graphic;

#endif //graphic_system_h