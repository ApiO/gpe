#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include "gpr_memory.h"
#include "gpr_types.h"
#include "gpr_tree.h"
#include "gpr_math.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  Matrix4 local_pose,  world_pose;
  I32     local_depth, world_depth;
  AABB    aabb;
  F32     width, height;
  Vector2 center;
  U64     tex_id;
  Vector2 tex_uv;
  I32     dirty;
} scene_graph_node_t;

typedef gpr_array_t(scene_graph_node_t) scene_graph_buffer_t;

typedef struct gpr_tree_t scene_graph_t;

void scene_graph_t_init     (scene_graph_t *s, gpr_allocator_t *a);
U64  scene_graph_t_add      (scene_graph_t *s);
void scene_graph_t_cull     (scene_graph_t *s, scene_graph_buffer_t *out);
void scene_graph_t_shutdown (scene_graph_t *s);


#ifdef __cplusplus
}
#endif

#endif //SCENE_GRAPH_H