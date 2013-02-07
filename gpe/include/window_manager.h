#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#define GLEW_STATIC
#include <GL\glew.h>
#include "font_system.h"
#include "gpr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  double  t0Value;
  int     fpsFrameCount;
  U64     id;
  I32     fps;
  I32     last_fps;
} fps_util_t;

typedef struct window_manager
{
  int running;
  int restart;
  U32 axes_cmd;
  I32 width, height;
  I32 display_axes, display_fps;
  fps_util_t fps_util;
} window_manager;

void window_manager_init        (window_manager *m, char * title, I32 height, I32 width);
void window_manager_clear       (window_manager *m);
void window_manager_swapBuffers (window_manager *m);
void window_manager_free        (window_manager *m);

#ifdef __cplusplus
}
#endif

#endif //window_manager_h