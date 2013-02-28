#ifndef _ressource_manager_h_
#define _ressource_manager_h_

#include "gpr_types.h"
#include "gpr_math.h"
#include "gpr_idlut.h"

#ifdef __cplusplus
extern "C" {
#endif
  
typedef struct
{
  U64 id;
  U32 width, height;
} gpe_texture_t;

typedef struct
{
  U64     id;
  U64     tex_id;
  Vector2 vertices[4];
  F32     u, v;
  U32     depth;
} gpe_graphic_t;

typedef struct
{
  gpr_idlut_t graphics;
  gpr_idlut_t textures;
} ressource_manager;

void ressource_manager_init           (ressource_manager *r);
U64  ressource_manager_add_graphic    (ressource_manager *r, gpe_graphic_t *g);
void ressource_manager_remove_graphic (ressource_manager *r, U64 id);
U64  ressource_manager_add_texture    (ressource_manager *r, gpe_texture_t *t);
void ressource_manager_remove_texture (ressource_manager *r, U64 id);
void ressource_manager_destroy        (ressource_manager *r);

#ifdef __cplusplus
}
#endif

#endif