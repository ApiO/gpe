#ifndef RSX_MNGR_TEMP_H
#define RSX_MNGR_TEMP_H

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
  U64    id;
  U64    tex_id;
  Point2 vertices[4];
  F32    u, v;
  U32    depth;
} gpe_graphic_t;

typedef struct
{
  gpr_idlut_t sprites;
} rsx_mngr;

void rsx_mngr_temp_init(rsx_mngr *r);
U64  rsx_mngr_temp_add_sprite(rsx_mngr *r, gpe_graphic_t *s);
void rsx_mngr_temp_remove_sprite(rsx_mngr *r, U64 ie);
void rsx_mngr_temp_destroy(rsx_mngr *r);


#ifdef __cplusplus
}
#endif


#endif //RSX_MNGR_TEMP_H