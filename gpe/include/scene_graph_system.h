#ifndef SCENE_GRAPH_SYSTEM_H
#define SCENE_GRAPH_SYSTEM_H

#include "gpr_memory.h"
#include "gpr_types.h"
#include "gpr_tree.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  U32 i;
} sg_tree_value_t;

typedef struct
{
  U32 i;
} sg_buffer_item_t;
typedef gpr_array_t(sg_buffer_item_t) sg_graphic_buffer;

typedef struct
{
  gpr_tree_t  scene_graph;
  gpr_idlut_t graphic_buffer;
} scene_graph_system;


void scene_graph_system_init     (scene_graph_system *s, gpr_allocator_t *a);
U64  scene_graph_system_add      (scene_graph_system *s, U64 id);
void scene_graph_system_set      (scene_graph_system *s, U64 id);
void scene_graph_system_render 	 (scene_graph_system *s);
void scene_graph_system_destroy  (scene_graph_system *s, U64 id);
void scene_graph_system_shutdown (scene_graph_system *s);


#ifdef __cplusplus
}
#endif

#endif //SCENE_GRAPH_SYSTEM_H