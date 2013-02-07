#include "renderer_test.h"

#include <math.h>
#include <time.h>

#include "window_manager.h"
#include <SOIL\SOIL.h>
#include "rsx_mngr_temp.h"
#include "renderer.h"
#include "gpr_memory.h"

#define HEIGHT          600
#define WIDTH           800
#define ITEM_COUNT      5
#define ITEM_TEX_COUNT  2


void _init_env(graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex);

void renderer_test_foo()
{
  int i;
  window_manager w;
  graphic_buffer gb;
  rsx_mngr r;
  U32 soil_tex[ITEM_TEX_COUNT];

  gpr_memory_init(410241024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_fps = 1;
  w.display_axes = 1;

  _init_env(&gb, &r, soil_tex);

  while(w.running)
  {
    window_manager_clear(&w);
      
    renderer_draw(&gb);
      
    window_manager_swapBuffers(&w);
  }

  //clean
  for (i=0; i < ITEM_TEX_COUNT; i++)  glDeleteTextures(1, &(GLuint)soil_tex[i]);
  gpr_array_destroy(&gb);
  rsx_mngr_temp_destroy(&r);
  window_manager_free(&w);
  gpr_memory_shutdown();
}

void _init_env(graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex)
{
  int i;
 // U32 gl_cmd_id;
 // U64 cmd_id;

  gpr_array_init(gpe_scene_item_t, gb, gpr_default_allocator);
  rsx_mngr_temp_init(rm);

  soil_tex[0] = SOIL_load_OGL_texture (
    "D:\\temp\\patate.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  soil_tex[1] = SOIL_load_OGL_texture (
    "D:\\temp\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  //create gl command list and get list ID
  //gl_cmd_id = glNewList(1); ...
  //cmd_id = rsx_mngr_temp_add_command(rm, gl_cmd_id);
  
  for(i=0; i < ITEM_COUNT; i++)
  {
    gpe_scene_item_t scene_item;
    gpe_sprite_t sprite;
    U64 sprite_id;

    sprite.tex_id = soil_tex[i%2 ? 1 : 0];
    //sprite.command_id = cmd_id;
    sprite.tex_x = 0;
    sprite.tex_y = 0;
    sprite.height = 128;
    sprite.width = 128;

    sprite_id = rsx_mngr_temp_add_sprite(rm, &sprite);

    scene_item.sprite_id = sprite_id;
    scene_item.world_transform = 0;

    gpr_array_push_back(gpe_scene_item_t, gb, scene_item);
  }
}