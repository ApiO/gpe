#include "graphic_system_test.h"

#include "window_manager.h"
#include "graphic_system.h"
#include <SOIL\SOIL.h>


void graphic_system_test_foo(void)
{

  window_manager manager;
  graphic_system graphics;
  U32 patateId;
  GLuint tex_id;
  
  window_manager_init(&manager, "graphic debug", 600, 800);
  
  gpr_memory_init(4*1024*1024);

  //graphic inits
  graphic_system_init(&graphics, 4);

  tex_id = SOIL_load_OGL_texture (
    "D:\\temp\\patate.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  patateId = graphic_system_add(&graphics, tex_id);

  gpe_graphic *graphic = graphic_system_lookup(&graphics, patateId);

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