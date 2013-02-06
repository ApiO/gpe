#ifndef RENDERER_H
#define RENDERER_H

#include "gpr_tmp_allocator.h"
#include "gpr_types.h"
#include "gpr_array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  U64 world_transform;
  U64 sprite_id;
  U64 shader_id;
} gpe_scene_item_t;


typedef gpr_array_t(gpe_scene_item_t) graphic_buffer;


void renderer_draw(graphic_buffer *gb);

#ifdef __cplusplus
}
#endif

#endif //RENDERER_H