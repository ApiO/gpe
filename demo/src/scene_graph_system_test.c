#include "scene_graph_system_test.h"
#include "scene_graph_system.h"

#include <stdio.h>
#include "window_manager.h"
#include "renderer.h"
#include "rsx_mngr_temp.h"
#include <SOIL\SOIL.h>

#define WIDTH       1600
#define HEIGHT      1000
#define ITEM_COUNT  14

void _init_rsx(rsx_mngr *r);
void _load_sprite (rsx_mngr *r, char *path, F32 width, F32 height, F32 local_trans_x, F32 local_trans_y, U32 local_depth);
void _init_graphic_buffer(rsx_mngr *r, graphic_buffer *gb);
void _init_graphics(rsx_mngr *r, graphic_buffer *gb, F32 world_trans_x, F32 world_trans_y, U32 world_depth);

static U64 _sprites[ITEM_COUNT];

void foo()
{
  window_manager w;
  rsx_mngr r;
  graphic_buffer gb;
  gpr_memory_init(4*1024*1024);
  
  window_manager_init(&w, "rendering : sort test", HEIGHT, WIDTH);
  w.display_fps = 1;
  w.display_axes = 1;
  
  gpr_array_init(gpe_scene_item_t, &gb, gpr_default_allocator);
  rsx_mngr_temp_init(&r);
  
  _init_rsx(&r);
  _init_graphic_buffer(&r, &gb);

  renderer_init();
  while(w.running)
  {
    window_manager_clear(&w);
      
    renderer_draw(&r, &gb);

    window_manager_swapBuffers(&w);
  }
  
  renderer_shutdown();
  gpr_array_destroy(&gb);
  rsx_mngr_temp_destroy(&r);
  window_manager_free(&w);
  gpr_memory_shutdown();
}


void _init_rsx(rsx_mngr *r)
{
  //0
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_shadows\\shadow_idle.png", 321,  64, -158,  27,   13);
  //1
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\shoulder_a.png",     71,   115, 62,    306,  12);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\hand_a_0.png",       104,  113, 154,   266,  11);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\forearm_a.png",      99,   78 , 84,    246,  10);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\shoulder_0.png",     114,  94 , -93,   350,  5);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\forearm_0.png",      84,   108, -133,  316,  4);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\hand_0_0.png",       110,  117, -154,  247,  3);
  //2
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\foot_a.png",         134,  162, 31,   138,  9);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\thigh_a.png",        126,  112, 8,    194,  8);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\pelvis_0.png",       110,  149, -56,  238,  7);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\foot_0.png",         139,  159, -151, 137,  2);
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\thigh_0.png",        81,   156, -74,  222,  1);
  //3
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_torso\\torso_0.png",       180,  197, -66,  375,  6);
  //4
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_head\\head_0.png",         144,  151, -8 ,  410,  0);
}

void _load_sprite (rsx_mngr *r, char *path, F32 width, F32 height, F32 local_trans_x, F32 local_trans_y, U32 local_depth)
{
  gpe_sprite_t sprite;
  U32 tex_id;
  
  sprite.tex_id = SOIL_load_OGL_texture (
    path, 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  if(sprite.tex_id == 0)
  {
    fprintf(stderr, "SOIL - file not found : %s\n", path);
    exit(EXIT_FAILURE); 
  }

  sprite.tex_x = 0;
  sprite.tex_y = 0;
  sprite.tex_h = height;
  sprite.tex_w = width;
  sprite.height = height;
  sprite.width = width;
  sprite.local_depth = local_depth;
  sprite.local_translate.x = local_trans_x;
  sprite.local_translate.y = local_trans_y*-1;

  renderer_init_vbo(&sprite);

  _sprites[local_depth] = rsx_mngr_temp_add_sprite(r, &sprite);
}

void _init_graphic_buffer(rsx_mngr *r, graphic_buffer *gb)
{
  _init_graphics(r, gb, 800,  400, 0);
  _init_graphics(r, gb, 600,  500, 1);
  _init_graphics(r, gb, 1000, 500, 2);
  _init_graphics(r, gb, 800,  600, 3);
  
  _init_graphics(r, gb, 200,  950, 4);
  _init_graphics(r, gb, 300,  950, 5);
  _init_graphics(r, gb, 400,  950, 6);
  _init_graphics(r, gb, 500,  950, 7);

}

void _init_graphics(rsx_mngr *r, graphic_buffer *gb, F32 world_trans_x, F32 world_trans_y, U32 world_depth)
{
  int i;

  for(i=0; i<ITEM_COUNT; i++)
  {
    gpe_sprite_t *spite = gpr_idlut_lookup(gpe_sprite_t, &r->sprites, _sprites[i]);
    gpe_scene_item_t si;
    si.sprite_id = _sprites[i];
    si.translate.x = spite->local_translate.x + world_trans_x;
    si.translate.y = spite->local_translate.y + world_trans_y;
    si.translate.z = spite->local_depth;
    si.scale.x = 1.f;
    si.scale.y = 1.f;
    si.scale.z = 1.f;
    si.world_depth = world_depth;

    gpr_array_push_back(gpe_scene_item_t, gb, si);
  }
}