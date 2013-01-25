#include "rsx_mngr_temp.h"

void rsx_mngr_temp_init(rsx_mngr *r)
{
  gpr_idlut_init(gpe_sprite_t, &r->sprites, gpr_default_allocator);
  gpr_idlut_init(U32, &r->sprite_commands, gpr_default_allocator);
}

U64 rsx_mngr_temp_add_sprite(rsx_mngr *r, gpe_sprite_t *s)
{
  return gpr_idlut_add(gpe_sprite_t, &r->sprites, s);
}

void rsx_mngr_temp_remove_sprite(rsx_mngr *r, U64 ie)
{
  gpr_idlut_remove(gpe_sprite_t, &r->sprites, ie);
}

U64 rsx_mngr_temp_add_command(rsx_mngr *r, U32 c)
{
  return gpr_idlut_add(U32, &r->sprite_commands, &c);
}

void rsx_mngr_temp_command(rsx_mngr *r, U64 ie)
{
  gpr_idlut_remove(U32, &r->sprite_commands, ie);
}

void rsx_mngr_temp_destroy(rsx_mngr *r)
{
  gpr_idlut_destroy(gpe_sprite_t, &r->sprites);
  gpr_idlut_destroy(U32, &r->sprite_commands);
}