#ifndef window_manager_h
#define window_manager_h

#include "glbmfont.h"

typedef struct window_manager
{
  int     running;
  int     restart;
  double  fps;
  bool    display_fps;
} window_manager;

void window_manager_init        (window_manager *m, char * title, int height, int width);
void window_manager_clear       (window_manager *m);
void window_manager_swapBuffers (window_manager *m);
void window_manager_free        (window_manager *m);
void window_manager_print       (window_manager *m, char * t, int x, int y, gpe_dock d);
#endif //window_manager_h