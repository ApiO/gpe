#include "graphic_system_test.h"

#include <SOIL\SOIL.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "window_manager.h"
#include "graphic_system.h"

#include <stdio.h>

void graphic_system_test_foo(void)
{
  #define ITEM_COUNT 10000
  #define ITEM_TEX_COUNT 2
  int i;
  window_manager manager;
  graphic_system graphics;
  gpe_graphic   *graphic;
  U64            ids[ITEM_COUNT];
  GLuint         tex_id[ITEM_TEX_COUNT];
  

  window_manager_init(&manager, "graphic debug", 600, 800);
  
  gpr_memory_init(4*1024*1024);

  //graphic inits
  graphic_system_init(&graphics);
  
  tex_id[0] = SOIL_load_OGL_texture (
    "D:\\temp\\patate.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  tex_id[1] = SOIL_load_OGL_texture (
    "D:\\temp\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );
    
  printf("---[before sorting]\n");
  for(i=0; i < ITEM_COUNT; i++)
  {
    ids[i] = graphic_system_add(&graphics, tex_id[i%2 ? 1 : 0]);

    graphic = graphic_system_lookup(&graphics, ids[i]);
    graphic->x = (F32)(-64+600*cos(2*M_PI/ITEM_COUNT*i));
    graphic->y = (F32)(-64+400*sin(2*M_PI/ITEM_COUNT*i));
    graphic->z = i%3 ? 2 : i%2 ? 1 : 0;
    graphic->w = 128;
    graphic->h = 128;
    graphic->dev = i;

    //printf("z = %d | tex_id = %d\n", graphic->z, graphic->tex_id);
  }

  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear(&manager);
    
    graphic_system_update(&graphics);
    graphic_system_render(&graphics);

    window_manager_swapBuffers(&manager);
  }

  for (i=0; i < ITEM_TEX_COUNT; i++)  glDeleteTextures(1, &tex_id[i]);

  graphic_system_free(&graphics);
  gpr_memory_shutdown();
  window_manager_free(&manager);
}