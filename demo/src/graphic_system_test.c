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
  #define ITEM_COUNT 10
  #define ITEM_TEX_COUNT 2
  int i;
  window_manager manager;
  graphic_system graphics;
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
    
  for(i=0; i < ITEM_COUNT; i++)
  {
    gpe_graphic graphic;
    graphic.tex_id = tex_id[i%2 ? 1 : 0];
    graphic.x = (F32)(-64+600*cos(2*M_PI/ITEM_COUNT*i));
    graphic.y = (F32)(-64+400*sin(2*M_PI/ITEM_COUNT*i));
    graphic.z = i%3 ? 2 : i%2 ? 1 : 0;
    graphic.w = 128;
    graphic.h = 128;

    graphic_system_add(&graphics, graphic);
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

/*
void graphic_system_test_foo2(void)
{
  #define LOOP_COUNT 1000
  #define TEST_ITEM_BASE 1000
  #define TEST_ITEM_INCR_COUNT 30
  #define TEST_ITEM_INCR_VAL 1000
  #define ITEM_TEX_COUNT 2

  int i, j, ct;
  graphic_system  graphics_1;
  graphic_system  graphics_2;
  clock_t         start;

  GLuint         tex_id[ITEM_TEX_COUNT];
  
  gpr_memory_init(4*1024*1024);

  //graphic inits
  graphic_system_init(&graphics_1);
  graphic_system_init(&graphics_2);
  
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
        
        
  for (i=0; i < TEST_ITEM_INCR_COUNT; i++)
  {
    ct = TEST_ITEM_BASE + TEST_ITEM_INCR_VAL * i;
    for (j=0; j < LOOP_COUNT; j++) 
    {
      _graphic_system_test_foo2_init(&graphics_1, &graphics_2, ct, tex_id);

      start = clock();
      graphic_system_update(&graphics_1);
      _dev_clock_diff(start, clock());

      start = clock();
      graphic_system_render(&graphics_1);
      _dev_clock_diff(start, clock());
    
      start = clock();
      graphic_system_update(&graphics_2);
      _dev_clock_diff(start, clock());

      start = clock();
      graphic_system_render(&graphics_2);
      _dev_clock_diff(start, clock());

      graphic_system_free(&graphics_1);
      graphic_system_free(&graphics_2);
    }
  }
  
  //write file

  for (i=0; i < ITEM_TEX_COUNT; i++)  glDeleteTextures(1, &tex_id[i]);
  
  gpr_memory_shutdown();
}

void _graphic_system_test_foo2_init(graphic_system *graphics_1, graphic_system *graphics_2, int ct, GLuint tex_id[])
{
  int i;
  gpe_graphic graphic;

  for(i=0; i < ct; i++)
  {
    graphic.tex_id = tex_id[i%2 ? 1 : 0];
    graphic.x = (F32)(-64+600*cos(2*M_PI/ct*i));
    graphic.y = (F32)(-64+400*sin(2*M_PI/ct*i));
    graphic.z = i%3 ? 2 : i%2 ? 1 : 0;
    graphic.w = 128;
    graphic.h = 128;

    graphic_system_add(graphics_1, tex_id[i%2 ? 1 : 0]);
    graphic_system_add(graphics_2, tex_id[i%2 ? 1 : 0]);
  }
}*/

F64 _dev_clock_diff(clock_t start, clock_t end)
{
  return (F64)(end - start) / CLOCKS_PER_SEC;
}

