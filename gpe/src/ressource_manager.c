#include "ressource_manager.h"

void ressource_manager_init(ressource_manager *r)
{
  gpr_idlut_init(gpe_graphic_t, &r->graphics, gpr_default_allocator);
  gpr_idlut_init(gpe_texture_t, &r->textures, gpr_default_allocator);
}

U64 ressource_manager_add_graphic(ressource_manager *r, gpe_graphic_t *g)
{
  return gpr_idlut_add(gpe_graphic_t, &r->graphics, g);
}

void ressource_manager_remove_graphic(ressource_manager *r, U64 id)
{
  gpr_idlut_remove(gpe_graphic_t, &r->graphics, id);
}

U64 ressource_manager_add_texture(ressource_manager *r, gpe_texture_t *t)
{
  return gpr_idlut_add(gpe_texture_t, &r->textures, t);
}

void ressource_manager_remove_texture(ressource_manager *r, U64 id)
{
  gpr_idlut_remove(gpe_texture_t, &r->textures, id);
}

void ressource_manager_destroy(ressource_manager *r)
{
  gpr_idlut_destroy(gpe_graphic_t, &r->graphics);
  gpr_idlut_destroy(gpe_texture_t, &r->textures);
}