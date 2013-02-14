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
  U32     tex_id;
  U32     vbo[2];
  F32     tex_x, tex_y;
  F32     tex_w, tex_h;
  F32     width, height;
  U32     local_depth;
  Vector2 local_translate;
} gpe_sprite_t;

typedef struct
{
  gpr_idlut_t sprites;
} rsx_mngr;

void rsx_mngr_temp_init(rsx_mngr *r);
U64  rsx_mngr_temp_add_sprite(rsx_mngr *r, gpe_sprite_t *s);
void rsx_mngr_temp_remove_sprite(rsx_mngr *r, U64 ie);
void rsx_mngr_temp_destroy(rsx_mngr *r);


#ifdef __cplusplus
}
#endif


#endif //RSX_MNGR_TEMP_H