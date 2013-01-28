#include "font_system_test.h"

#include "window_manager.h"
#include "font_system.h"

const int HEIGHT = 600;
const int WIDTH = 800;

void font_system_test_foo()
{
  window_manager w;
  U64 tid;

  window_manager_init(&w, "FONT_SYSTEM debug", HEIGHT, WIDTH);
  w.display_axes = true;
  
  gpr_memory_init(4*1024*1024);

  font_system_init();

  tid = font_system_text_init();

  while(w.running)
  {
    window_manager_clear(&w);
    
    font_system_text_print(tid, "toto", 0, 0, dock_center2);

    window_manager_swapBuffers(&w);
  }

  font_system_text_remove(tid);

  font_system_free();
  window_manager_free(&w);
  gpr_memory_shutdown();
}
