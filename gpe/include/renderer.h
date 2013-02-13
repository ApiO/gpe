#ifndef RENDERER_H
#define RENDERER_H

#include "gpr_tmp_allocator.h"
#include "gpr_types.h"
#include "gpr_array.h"
#include "rsx_mngr_temp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  F32 x, y;
} vec2F32;

typedef struct
{
  F32 x, y, z;
} _3F32;

typedef struct
{
  U64     sprite_id;
  _3F32   translate;
  _3F32   scale;
  vec2F32 aabb[2];
  wchar_t userData[50];
} gpe_scene_item_t;
typedef gpr_array_t(gpe_scene_item_t) graphic_buffer;

void renderer_init();
void renderer_draw (rsx_mngr *r, graphic_buffer *gb);
void renderer_init_vbo (gpe_sprite_t *s);
void renderer_destroy_vbo (gpe_sprite_t *s);
void renderer_shutdown();

#ifdef __cplusplus
}
#endif

#endif //RENDERER_H