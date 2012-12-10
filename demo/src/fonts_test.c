#include "fonts_test.h"

#include <stdio.h>
#include "window_manager.h"
#include "glbmfont.h"

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 640;

void fonts_test_foo(void)
{
  window_manager manager;
  /*
  char buffer[255];
  double fps;
  */
  window_manager_init(&manager, "FONTS debug", SCREEN_HEIGHT, SCREEN_WIDTH);

  glbmfont_load();

  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear();

    //stuff
    /*
    fps = window_manager_getFps();
    sprintf_s(buffer, "%.0f", fps);
    glbmfont_print(buffer, 10, 10);
    */
    glbmfont_print("A", 10, 10);

    window_manager_swapBuffers(&manager);
  }
  
  glbmfont_free();
  window_manager_free(&manager);
}