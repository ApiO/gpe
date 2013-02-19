#ifndef RENDERER_H
#define RENDERER_H

#include "gpr_tmp_allocator.h"
#include "gpr_types.h"
#include "gpr_array.h"
#include "ressource_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  F32 x1, y1, x2, y2;
} AABB;

typedef struct
{
  U64     sprite_id;
  U32     world_depth;
  Vector3 translate;
  Vector3 scale;
} gpe_scene_item_t;
typedef gpr_array_t(gpe_scene_item_t) graphic_buffer;

typedef struct
{
  Vector3 translate;
  Vector3 scale;
  U32     vbo[3];
  U32     vao;
  U32     tex_id;
  U32     world_depth, local_depth;
} gpe_render_item_t;
typedef gpr_array_t(gpe_render_item_t) render_buffer;

typedef struct
{
  U32 vbo[3];
} renderer_graphic_t;

typedef struct
{
  U32 tex_id;
} renderer_texture_t;

typedef struct
{
  ressource_manager *rm;
  render_buffer render_buffer;
  gpr_idlut_t graphics;
  gpr_idlut_t textures;

} renderer;

void renderer_init(renderer *r, ressource_manager *rm);
void renderer_draw            (renderer *r, graphic_buffer *gb);
U64  renderer_init_graphic    (renderer *r, gpe_graphic_t *g);
void renderer_destroy_graphic (renderer *r, U64 id);
U64  renderer_init_texture    (renderer *r, char *path);
void renderer_destroy_texture (renderer *r, U64 id);
void renderer_shutdown(renderer *r);

#ifdef __cplusplus
}
#endif

#endif //RENDERER_H