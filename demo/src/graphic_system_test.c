#include "graphic_system_test.h"

#include "window_manager.h"
#include "graphic_system.h"


void graphic_system_test_foo(void)
{

  window_manager manager;
  graphic_system graphics;
  
  window_manager_init(&manager, "graphic debug", 600, 800);
  
  //graphic inits
  graphic_system_init(&graphics, 4);

  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear(&manager);

    //graphic draw
    graphic_system_render(&graphics);

    window_manager_swapBuffers(&manager);
  }
  
  window_manager_free(&manager);
}