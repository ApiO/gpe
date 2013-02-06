#include "graphic_system_test.h"

#include <SOIL\SOIL.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#include "window_manager.h"
#include "graphic_system.h"

#include <stdio.h>


F64 _dev_clock_diff(clock_t start, clock_t end);
void _graphic_system_test_foo2_init(graphic_system *graphics_1, graphic_system *graphics_2, int ct, GLuint tex_id[]);

void graphic_system_test_foo(void)
{
  #define ITEM_COUNT 5
  #define ITEM_TEX_COUNT 2
  int i;
  window_manager w;
  graphic_system graphics;
  GLuint         tex_id[ITEM_TEX_COUNT];
  
  window_manager_init(&w, "graphic debug", 600, 800);
  
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
    
  for(i=0; i < ITEM_COUNT; i++)
  {
    gpe_graphic_t graphic;
    graphic.tex_id = tex_id[i%2 ? 1 : 0];
    graphic.x = (F32)(-64+600*cos(2*M_PI/ITEM_COUNT*i));
    graphic.y = (F32)(-64+400*sin(2*M_PI/ITEM_COUNT*i));
    graphic.z = i%3 ? 2 : i%2 ? 1 : 0;
    graphic.w = 128;
    graphic.h = 128;

    graphic_system_add_d(&graphics, graphic);
  }

  //pseudo game loop
  while (w.running)
  {
    window_manager_clear(&w);
    
    graphic_system_update(&graphics);
    graphic_system_render(&graphics);

    window_manager_swapBuffers(&w);
  }

  for (i=0; i < ITEM_TEX_COUNT; i++)  glDeleteTextures(1, &tex_id[i]);

  graphic_system_free(&graphics);
  gpr_memory_shutdown();
  window_manager_free(&w);
}

F64 _dev_clock_diff(clock_t start, clock_t end)
{
  return (F64)(end - start) / CLOCKS_PER_SEC;
}

