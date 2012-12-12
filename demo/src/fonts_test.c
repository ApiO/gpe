#include "fonts_test.h"

#include <stdio.h>
#include "window_manager.h"
#include "glbmfont.h"

const int SCREEN_HEIGHT = 400;
const int SCREEN_WIDTH = 1000;

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

    /*
    //stuff

    fps = window_manager_getFps();
    sprintf_s(buffer, "FPS: %f", fps);
    glbmfont_print(buffer, 0, 0);
    */
    //*
    glbmfont_print("abcdefghijklmno\npqrstuvwxyz", 10, 10, dock_top_left);
    glbmfont_print("ABCDEF\nGHIJKLMNOPQRSTUVWXYZ", 10, 10, dock_top_right);
    glbmfont_print("0123456789°+\nqdkjfsqkdfsqsdf\ndfg\nklmm", 0, 0, dock_center);
    glbmfont_print("&é\"'(-è_ç\nà)=^$*ù!:;,<ïöüäâîôû", 10, 10, dock_bottom_left);
    glbmfont_print("~#{[|`\\^\n@]}¤²", 10, 10, dock_bottom_right);
    //*/
    
    //glbmfont_print("bas droit", 0, 0, dock_bottom_left);
    /*
    glbmfont_print("bas droit", 0, 0, dock_bottom_right);
    */

    DEV_window_manager_drawAxes();

    window_manager_swapBuffers(&manager);
  }
  
  glbmfont_free();
  window_manager_free(&manager);
}