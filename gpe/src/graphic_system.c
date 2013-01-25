#include "graphic_system.h"

#include "gpr_memory.h"

void _graphic_system_bind_gl_item (gpe_graphic_t *graphic, gpe_gl_graphic_t *gl_graphic);

void graphic_system_init (graphic_system *system)
{
  gpr_idlut_init(gpe_graphic_t, &(system->table), gpr_default_allocator);
}

U64 graphic_system_add (graphic_system *system, GLuint tex_id)
{
  gpe_graphic_t graphic;
  graphic.tex_id = tex_id;
  return gpr_idlut_add(gpe_graphic_t, &system->table, &graphic);
}

U64 graphic_system_add (graphic_system *system, gpe_graphic_t graphic)
{
  gpe_gl_graphic_t gl_graphic;
  _graphic_system_bind_gl_item(&graphic, &gl_graphic);
  return gpr_idlut_add(gpe_graphic_t, &system->table, &graphic);
}

void graphic_system_remove (graphic_system *system, U64 graphic_id)
{
  gpr_idlut_remove(gpe_graphic_t, &system->table, graphic_id);
}

gpe_graphic_t* graphic_system_lookup (graphic_system *system, U64 graphic_id)
{
  return gpr_idlut_lookup(gpe_graphic_t, &system->table, graphic_id);
}

void graphic_system_update (graphic_system *system)
{
  if (system->table.num_items <= 0) return;
  
  //

  gl_renderer_update(&system->renderer);
}

void graphic_system_render (graphic_system *system)
{
  gl_renderer_draw(&system->renderer);
}

void graphic_system_free (graphic_system *system)
{
  gpr_idlut_destroy(gpe_graphic, &system->table);
  gl_renderer_free(&system->renderer);
}

void _graphic_system_bind_gl_item(gpe_graphic_t *graphic, gpe_gl_graphic_t *gl_graphic)
{
  gl_graphic->tex_id = graphic->tex_id;
  gl_graphic->x = graphic->x;
  gl_graphic->y = graphic->y;
  gl_graphic->w = graphic->w;
  gl_graphic->h = graphic->h;
  gl_graphic->scale = graphic->scale;
  gl_graphic->r = graphic->r;
  gl_graphic->a = graphic->a;
  gl_graphic->shear_x = graphic->shear_x;
  gl_graphic->shear_y = graphic->shear_y;
  gl_graphic->z = graphic->z;
}