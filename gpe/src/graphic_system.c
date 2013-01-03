#include "graphic_system.h"

U32 graphic_system_add (gpe_graphic *gData, U32 tex_id)
{
  return 0;
}

void graphic_remove (gpe_graphic *gData, U32 tex_id)
{
}

void graphic_system_render (gpe_graphic *gData)
{
  /*
  sort par gpe_graphic->z puis gpe_graphic->tex_id
  parser par z ascendant pour limiter le changement de texture lors du changement de z
  puis faire le rendu par z descendant
  */
}