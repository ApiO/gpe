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
  F32 x, y, z;
} _3F32;

typedef struct
{
  F32 x1, y1, x2, y2;
} AABB;

typedef struct
{
  U64     sprite_id;
  _3F32   translate;
  _3F32   scale;
  AABB    aabb;
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