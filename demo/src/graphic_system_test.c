#include "graphic_system_test.h"

#include <SOIL\SOIL.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "window_manager.h"
#include "graphic_system.h"


void graphic_system_test_foo(void)
{
  const int SIZE = 20;
  int i;
  window_manager manager;
  graphic_system graphics;
  U32 patate_ids[SIZE];
  GLuint tex_id;
  
  window_manager_init(&manager, "graphic debug", 600, 800);
  
  gpr_memory_init(4*1024*1024);

  //graphic inits
  graphic_system_init(&graphics, SIZE);

  tex_id = SOIL_load_OGL_texture (
    "D:\\temp\\patate.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  for(i=0; i < SIZE; i++)
  {
    patate_ids[i] = graphic_system_add(&graphics, tex_id);

    gpe_graphic *graphic = graphic_system_lookup(&graphics, patate_ids[i]);
    graphic->x = -64+600*cos(2*M_PI/SIZE*i);
    graphic->y = -64+400*sin(2*M_PI/SIZE*i);
    graphic->z = 0;
    graphic->w = 128;
    graphic->h = 128;
    graphic->texCoord[TEX_TOP_LEFT].x = 0;
    graphic->texCoord[TEX_TOP_LEFT].y = 0;
    graphic->texCoord[TEX_BOTTOM_LEFT].x = 0;
    graphic->texCoord[TEX_BOTTOM_LEFT].y = 1;
    graphic->texCoord[TEX_BOTTOM_RIGHT].x = 1;
    graphic->texCoord[TEX_BOTTOM_RIGHT].y = 1;
    graphic->texCoord[TEX_TOP_RIGHT].x = 1;
    graphic->texCoord[TEX_TOP_RIGHT].y = 0; 
  }

  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear(&manager);

    //graphic draw
    graphic_system_render(&graphics);

    window_manager_swapBuffers(&manager);
  }
  
  glDeleteTextures(1, &tex_id);
  gpr_memory_shutdown();
  window_manager_free(&manager);
}