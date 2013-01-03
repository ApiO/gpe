#include "window_test.h"
#include "window_manager.h"

void window_test_foo(void)
{
  window_manager manager;
  
  window_manager_init(&manager, "yo hoho hoooooo", 600, 800);

  //Esc to close window (hard codé dans manager pour le moment)
  while (manager.running)
  {
    window_manager_clear(&manager);
    //stuff
    window_manager_swapBuffers(&manager);
  }
  
  window_manager_free(&manager);

}