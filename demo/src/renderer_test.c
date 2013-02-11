#include "renderer_test.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "window_manager.h"
#include <SOIL\SOIL.h>
#include "rsx_mngr_temp.h"
#include "renderer.h"
#include "gpr_memory.h"

#define HEIGHT          600
#define WIDTH           800
#define ITEM_COUNT      1000
#define ITEM_TEX_COUNT  2
#define TEX_HEIGHT      128
#define TEX_WIDTH       128

void _init_env(graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex);
void _load_sprite(rsx_mngr *rm, char *path, U32 *soil_tex, U64 *sprite_id, int i);

void renderer_test_foo()
{
  int i;
  window_manager w;
  graphic_buffer gb;
  rsx_mngr r;
  U32 soil_tex[ITEM_TEX_COUNT];

  //gpr_memory_init(410241024);

  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_fps = 1;
  w.display_axes = 1;

  _init_env(&gb, &r, soil_tex);

  while(w.running)
  {
    window_manager_clear(&w);
      
    renderer_draw(&r, &gb);
      
    window_manager_swapBuffers(&w);
  }

  //clean
  for (i=0; i < ITEM_TEX_COUNT; i++)  
  {
    glDeleteTextures(1, &(GLuint)soil_tex[i]);
    renderer_destroy_vbo((gpr_idlut_begin(gpe_sprite_t, &r.sprites)+i));
  }

  gpr_array_destroy(&gb);
  rsx_mngr_temp_destroy(&r);
  window_manager_free(&w);
  gpr_memory_shutdown();
}

void _init_env(graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex)
{
  int i;
  U64 sprite_id[2];

  //init rsx_mngr
  gpr_array_init(gpe_scene_item_t, gb, gpr_default_allocator);
  rsx_mngr_temp_init(rm);

  _load_sprite(rm, "..\\..\\src\\ressources\\patate.png", soil_tex, sprite_id, 0);  
  _load_sprite(rm, "..\\..\\src\\ressources\\patate2.png", soil_tex, sprite_id, 1);
  
  //init graphic buffer
  for(i=0; i < ITEM_COUNT; i++)
  {
    gpe_scene_item_t scene_item;
    scene_item.sprite_id = sprite_id[i%2 ? 1 : 0];
    scene_item.world_transform[0] = (F32)(WIDTH/2)+(-(TEX_WIDTH/2)+(WIDTH/3)*cos(2*M_PI/ITEM_COUNT*i));
    scene_item.world_transform[1] = (F32)(HEIGHT/2)+(-(TEX_HEIGHT/2)+(HEIGHT/3)*sin(2*M_PI/ITEM_COUNT*i));
    scene_item.world_transform[2] = i%3 ? 2 : i%2 ? 1 : 0;
    gpr_array_push_back(gpe_scene_item_t, gb, scene_item);
  }
}

void _load_sprite(rsx_mngr *rm, char *path, U32 *soil_tex, U64 *sprite_id, int i)
{
  gpe_sprite_t sprite;

  soil_tex[i] = SOIL_load_OGL_texture (
    path, 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  if(soil_tex[i] == 0)
  {
    fprintf(stderr, "SOIL - file not found : %s\n", path);
    exit(EXIT_FAILURE); 
  }

  sprite.tex_id = soil_tex[i];
  sprite.tex_x = 0;
  sprite.tex_y = 0;
  sprite.tex_h = TEX_HEIGHT;
  sprite.tex_w = TEX_WIDTH;
  sprite.height = TEX_HEIGHT;
  sprite.width = TEX_WIDTH;

  renderer_init_vbo(&sprite);

  sprite_id[i] = rsx_mngr_temp_add_sprite(rm, &sprite);
}