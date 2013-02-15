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

#include <stdint.h>

#define HEIGHT          600
#define WIDTH           800
#define ITEM_COUNT      1000
#define ITEM_TEX_COUNT  2
#define TEX_HEIGHT      128
#define TEX_WIDTH       128

typedef struct
{
  F32 x, y;
  U64 id;
} _text_pose;
typedef gpr_array_t(_text_pose) _text_poses;

void _init_env_1 (graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex);
void _init_env_2 (graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex);
void _load_sprite (rsx_mngr *rm, char *path, U32 *soil_tex, U64 *sprite_id, int i);
void _add_scene_item (graphic_buffer *gb, F32 x, F32 y, F32 z, F32 scale_x, F32 scale_y, U64 srite_id, wchar_t *str, U32 wd);

static _text_poses _tposes;

void _test_foo1();
void _test_foo2();

void renderer_test_foo()
{
  _test_foo1();
  //_test_foo2();
}

void _test_foo1()
{
  U32 i;
  U64 desc_id;
  wchar_t desc[500];
  window_manager w;
  graphic_buffer gb;
  rsx_mngr r;
  U32 soil_tex[ITEM_TEX_COUNT];

  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "renderer : test", HEIGHT, WIDTH);
  w.display_fps = 1;
  w.display_axes = 1;
  
  gpr_array_init(gpe_scene_item_t, &gb, gpr_default_allocator);
  rsx_mngr_temp_init(&r);

  _init_env_1(&gb, &r, soil_tex);

  desc_id = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  swprintf(desc, L"entities:%d\nsprites:%d\ngraphic buffer size:%d", ITEM_COUNT, ITEM_TEX_COUNT, gb.size);
  font_system_text_set(desc_id, desc, ALIGN_TEXT_LEFT);

  renderer_init();
  while(w.running)
  {
    window_manager_clear(&w);
      
    renderer_draw(&r, &gb);
    font_system_text_print(desc_id, 0, 0, DOCK_TEXT_BOTTOM_LEFT, HEIGHT, WIDTH);
      
    window_manager_swapBuffers(&w);
  }

  //clean
  for (i=0; i < ITEM_TEX_COUNT; i++)  
  {
    glDeleteTextures(1, &(GLuint)soil_tex[i]);
    renderer_destroy_vbo((gpr_idlut_begin(gpe_sprite_t, &r.sprites)+i));
  }
  
  renderer_shutdown();

  gpr_array_destroy(&gb);
  rsx_mngr_temp_destroy(&r);
  window_manager_free(&w);
  
  gpr_memory_shutdown();
}

void _test_foo2()
{
  U32 i;
  window_manager w;
  graphic_buffer gb;
  rsx_mngr r;
  U32 soil_tex[ITEM_TEX_COUNT];

  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "rendering : sort test", HEIGHT, WIDTH);
  w.display_fps = 1;
  w.display_axes = 1;
  
  gpr_array_init(gpe_scene_item_t, &gb, gpr_default_allocator);
  rsx_mngr_temp_init(&r);

  renderer_init();
  
  gpr_array_init(_text_pose, &_tposes, gpr_default_allocator);
  gpr_array_reserve(_text_pose, &_tposes, 8);
  _init_env_2(&gb, &r, soil_tex);

  while(w.running)
  {
    window_manager_clear(&w);
      
    renderer_draw(&r, &gb);
    for(i=0; i<_tposes.size; i++)
    {
      _text_pose *pose = &gpr_array_item(&_tposes, i);
      font_system_text_print(pose->id, pose->x, pose->y, DOCK_TEXT_TOP_LEFT, HEIGHT, WIDTH);
    }
      
    window_manager_swapBuffers(&w);
  }

  //clean
  for (i=0; i < ITEM_TEX_COUNT; i++)  
  {
    glDeleteTextures(1, &(GLuint)soil_tex[i]);
    renderer_destroy_vbo((gpr_idlut_begin(gpe_sprite_t, &r.sprites)+i));
  }
  
  renderer_shutdown();
  gpr_array_destroy(&gb);
  rsx_mngr_temp_destroy(&r);
  window_manager_free(&w);
  gpr_array_destroy(&_tposes);
  gpr_memory_shutdown();
}

void _init_env_1 (graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex)
{
  int i;
  U64 sprite_id[2];
  
  _load_sprite(rm, "..\\..\\src\\ressources\\patate.png", soil_tex, sprite_id, 0);  
  _load_sprite(rm, "..\\..\\src\\ressources\\patate2.png", soil_tex, sprite_id, 1);
  
  //init graphic buffer
  for(i=0; i < ITEM_COUNT; i++)
  {
    gpe_scene_item_t si;
    si.sprite_id = sprite_id[i%2 ? 1 : 0];
    si.translate.x = (F32)((WIDTH/2)+(-(TEX_WIDTH/2)+(WIDTH/3)*cos(2*M_PI/ITEM_COUNT*i)));
    si.translate.y = (F32)((HEIGHT/2)+(-(TEX_HEIGHT/2)+(HEIGHT/3)*sin(2*M_PI/ITEM_COUNT*i)));
    si.translate.z = (F32)(i%3 ? 2 : i%2 ? 1 : 0);
    si.scale.x = 1.f;
    si.scale.y = 1.f;
    si.scale.z = 1.f;
    si.world_depth = i;
    gpr_array_push_back(gpe_scene_item_t, gb, si);
  }
}

void _load_sprite (rsx_mngr *rm, char *path, U32 *soil_tex, U64 *sprite_id, int i)
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
  sprite.local_depth = 0;

  renderer_init_vbo(&sprite);

  sprite_id[i] = rsx_mngr_temp_add_sprite(rm, &sprite);
}

void _init_env_2 (graphic_buffer *gb, rsx_mngr *rm, U32 *soil_tex)
{
  U64 sprite_id[2];
  
  _load_sprite(rm, "..\\..\\src\\ressources\\t1.png", soil_tex, sprite_id, 0);  
  _load_sprite(rm, "..\\..\\src\\ressources\\t2.png", soil_tex, sprite_id, 1);
  
  _add_scene_item(gb, 200,30,5, 4,3, sprite_id[1], L"A", 0);
  _add_scene_item(gb, 584,60,4, 1,3, sprite_id[1], L"B", 1);
  _add_scene_item(gb, 392,60,4, .5f,1, sprite_id[0], L"E", 2);
  _add_scene_item(gb, 520,90,3, 1,1, sprite_id[0], L"D", 3);
  
  _add_scene_item(gb, 264,90,3, .5f,1, sprite_id[1], L"F", 4);
  _add_scene_item(gb, 584,120,2, 1,1, sprite_id[0], L"C", 5);
  _add_scene_item(gb, 10,120,2, .5f,3, sprite_id[0], L"G", 6);
  _add_scene_item(gb, 30,150,1, 1,3, sprite_id[1], L"H", 7);
  
}

void _add_scene_item(graphic_buffer *gb, F32 x, F32 y, F32 z, F32 scale_x, F32 scale_y, U64 sprite_id, wchar_t *str, U32 wd)
{
  _text_pose pose;
  gpe_scene_item_t si;
  si.sprite_id = sprite_id;
  si.translate.x = x;
  si.translate.y = y;
  si.translate.z = z;
  si.scale.x = scale_x;
  si.scale.y = scale_y;
  si.scale.z = 1.f;
  si.world_depth = wd;

  gpr_array_push_back(gpe_scene_item_t, gb, si);
  
  pose.id = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  font_system_text_set(pose.id, str, ALIGN_TEXT_LEFT);

  pose.x = x+5;
  pose.y = y;
  gpr_array_push_back(_text_pose, &_tposes, pose);
}