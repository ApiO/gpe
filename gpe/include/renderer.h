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
  U64 sprite_id;
  F32 world_transform[3];
} gpe_scene_item_t;
typedef gpr_array_t(gpe_scene_item_t) graphic_buffer;

void renderer_draw (rsx_mngr *r, graphic_buffer *gb);
void renderer_init_vbo (gpe_sprite_t *s);
void renderer_destroy_vbo (gpe_sprite_t *s);

#ifdef __cplusplus
}
#endif

#endif //RENDERER_H