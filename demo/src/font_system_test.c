#include "font_system_test.h"

#include "window_manager.h"
#include "font_system.h"

#define HEIGHT 600
#define WIDTH  800

void font_system_test_foo()
{
  window_manager w;
  U64 tid;

  window_manager_init(&w, "FONT_SYSTEM debug", HEIGHT, WIDTH);
  w.display_axes = true;
  
  gpr_memory_init(4*1024*1024);

  font_system_init(gpr_default_allocator);

  tid = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  font_system_text_set(tid, "putin de ta raçe : ça marche !\ndediou :)", ALIGN_TEXT_LEFT);

  while(w.running)
  {
    window_manager_clear(&w);
    
    font_system_text_print(tid, 0, 0, DOCK_TEXT_TOP_RIGHT, HEIGHT, WIDTH);

    window_manager_swapBuffers(&w);
  }

  font_system_text_destroy(tid);

  font_system_free();
  window_manager_free(&w);
  gpr_memory_shutdown();
}
