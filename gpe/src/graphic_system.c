#include "graphic_system.h"

void graphic_system_init (graphic_manager *system)
{
}

//return idlut id of the tex within the system
U32 graphic_system_add (graphic_manager *system, U32 tex_id)
{
  return 0;
}

void graphic_system_remove (graphic_manager *system, U32 graphic_id)
{
}

void graphic_system_render (graphic_manager *system)
{
  /*
  sort par gpe_graphic->z puis gpe_graphic->tex_id
  parser par z ascendant pour limiter le changement de texture lors du changement de z
  puis faire le rendu par z descendant
  */
}

void graphic_system_free (graphic_manager *system)
{
}
