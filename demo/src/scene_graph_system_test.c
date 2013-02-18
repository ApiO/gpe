#include "scene_graph_system_test.h"
#include "scene_graph_system.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include "window_manager.h"
#include "renderer.h"
#include "rsx_mngr_temp.h"
#include <SOIL\SOIL.h>

#define WIDTH         1200
#define HEIGHT        700
#define SPRITE_COUNT  14
#define ENTITY_COUNT  1

void _init_rsx(rsx_mngr *r);
void _load_sprite (rsx_mngr *r, char *path, F32 width, F32 height, 
                   F32 local_trans_x, F32 local_trans_y, U32 local_depth,
                   char user_data[50]);
void _init_graphic_buffer(rsx_mngr *r, graphic_buffer *gb);
void _init_graphics(rsx_mngr *r, graphic_buffer *gb, F32 world_trans_x, 
                    F32 world_trans_y, U32 world_depth);

static U64 _sprites[SPRITE_COUNT];
static I32 _curr_entities = 0;

void foo()
{
  window_manager w;
  rsx_mngr r;
  graphic_buffer gb;
  U64 desc_id;
  wchar_t desc[500];
  gpr_memory_init(4*1024*1024);
  
  window_manager_init(&w, "rendering : sort test", HEIGHT, WIDTH);
  w.display_fps = 1;
  w.display_axes = 1;
  
  gpr_array_init(gpe_scene_item_t, &gb, gpr_default_allocator);
  rsx_mngr_temp_init(&r);
  
  _init_rsx(&r);
  _init_graphic_buffer(&r, &gb);

  desc_id = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  swprintf(desc, L"entities: %d\ngraphic/entity: %d\ngraphic_buffer_size: %d", 
    _curr_entities, SPRITE_COUNT, gb.size);
  font_system_text_set(desc_id, desc, ALIGN_TEXT_LEFT);

  renderer_init();
  while(w.running)
  {
    window_manager_clear(&w);
      
    renderer_draw(&r, &gb);
    font_system_text_print(desc_id, 0, 0, DOCK_TEXT_BOTTOM_LEFT, HEIGHT, WIDTH);

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
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_shadows\\shadow_idle.png", 321,  64, -158,  27,   13, "shadow_idle");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\shoulder_a.png",     71,   115, 62,    306, 12, "shoulder_a");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\hand_a_0.png",       104,  113, 154,   266, 11, "hand_a_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\forearm_a.png",      99,   78 , 84,    246, 10, "forearm_a");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\shoulder_0.png",     114,  94 , -93,   350, 5,  "shoulder_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\forearm_0.png",      84,   108, -133,  316, 4,  "forearm_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_arms\\hand_0_0.png",       110,  117, -154,  247, 3,  "hand_0_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\foot_a.png",         134,  162, 31,   138,  9,  "foot_a");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\thigh_a.png",        126,  112, 8,    194,  8,  "thigh_a");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\pelvis_0.png",       110,  149, -56,  238,  7,  "pelvis_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\foot_0.png",         139,  159, -151, 137,  2,  "foot_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_legs\\thigh_0.png",        81,   156, -74,  222,  1,  "thigh_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_torso\\torso_0.png",       180,  197, -66,  375,  6,  "torso_0");
  _load_sprite (r, "..\\..\\src\\ressources\\monster\\mon_head\\head_0.png",         144,  151, -8 ,  410,  0,  "head_0");
}

void _load_sprite (rsx_mngr *r, char *path, F32 width, F32 height, 
                   F32 local_trans_x, F32 local_trans_y, U32 local_depth,
                   char user_data[50])
{
  gpe_sprite_t sprite;
  
  sprite.tex_id = SOIL_load_OGL_texture (
    path, 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  if(sprite.tex_id == 0)
  {
    char buffer[1024];
        sprintf(buffer, "\n----\nSOIL - file not found : %s\n\tline: %d\n\tfile: %s\n----\n", 
      path, __LINE__, __FILE__);
    OutputDebugString(buffer);
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
  //memcpy(sprite.user_data, user_data, 50);

  renderer_init_vbo(&sprite);

  _sprites[local_depth] = rsx_mngr_temp_add_sprite(r, &sprite);
}

void _init_graphic_buffer(rsx_mngr *r, graphic_buffer *gb)
{
  int i,
      start = 50,
      count = 50;
  /*
  _init_graphics(r, gb, 200,  425, 4);
  _init_graphics(r, gb, 250,  450, 3);
  _init_graphics(r, gb, 300,  475, 2);
  _init_graphics(r, gb, 350,  500, 1);
  _init_graphics(r, gb, 400,  525, 0);

  _init_graphics(r, gb, 800,  525, 5);
  _init_graphics(r, gb, 825,  500, 6);
  _init_graphics(r, gb, 850,  475, 7);
  _init_graphics(r, gb, 875,  450, 8);
  _init_graphics(r, gb, 900,  425, 9);
  */
  
  for(i=0; i<ENTITY_COUNT; i++)
  {
    F32 x = (F32)((WIDTH/2)+(WIDTH/5)*cos(2*M_PI/ENTITY_COUNT*i));
    F32 y = (F32)((HEIGHT*3/4)+(HEIGHT/6)*sin(2*M_PI/ENTITY_COUNT*i));
    _init_graphics(r, gb, x,  y, _curr_entities);
  }
}

void _init_graphics(rsx_mngr *r, graphic_buffer *gb, F32 world_trans_x, 
                    F32 world_trans_y, U32 world_depth)
{
  int i = SPRITE_COUNT-1;

  for(i=0; i<SPRITE_COUNT; i++)
  {
    gpe_sprite_t *spite = gpr_idlut_lookup(gpe_sprite_t, &r->sprites, 
                                          _sprites[i]);
    gpe_scene_item_t si;
    si.sprite_id = _sprites[i];
    si.translate.x = spite->local_translate.x + world_trans_x;
    si.translate.y = spite->local_translate.y + world_trans_y;
    si.translate.z = 0;
    si.scale.x = 1.f;
    si.scale.y = 1.f;
    si.scale.z = 1.f;
    si.world_depth = world_depth;

    gpr_array_push_back(gpe_scene_item_t, gb, si);
  }
  ++_curr_entities;
}