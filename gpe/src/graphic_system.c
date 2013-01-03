#include "graphic_system.h"

void graphic_system_init (graphic_system *system, U32 object_count)
{
  gpr_idlut_init(gpe_graphic, &system->table, object_count);
}

//return idlut id of the tex within the system
U32 graphic_system_add (graphic_system *system, U32 tex_id)
{
  gpe_graphic graphic = { tex_id };
  system->physics_count = system->physics_count + 1;
  return gpr_idlut_add(gpe_graphic, &system->table, &graphic);
}

void graphic_system_remove (graphic_system *system, U32 graphic_id)
{
  gpr_idlut_remove(gpe_graphic, &system->table, graphic_id);
}

void graphic_system_render (graphic_system *system)
{
  /*
  sort par gpe_graphic->z puis gpe_graphic->tex_id
  parser par z ascendant pour limiter le changement de texture lors du changement de z
  puis faire le rendu par z descendant
  */
  int i;
  gpe_graphic graphic;
  for(i=0; i < system->physics_count; i++)
  {
    graphic = (gpe_graphic)system->table.items[i].value;
    
    //init GL
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, graphic.text_id);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    //rendu de l'élément
    glBegin(GL_QUADS);
    {
      glTexCoord2f( 0, 0 );
 	    glVertex2d(   0, 0 );

      glTexCoord2f( 0, 0 );
 	    glVertex2d(   0, 0 );

      glTexCoord2f( 0, 0 );
 	    glVertex2d(   0, 0 );

      glTexCoord2f( 0, 0 );
 	    glVertex2d(   0, 0 );
	  } glEnd();
    
    //reset GL
    glDisable(GL_TEXTURE_2D);
  }
}

void graphic_system_free (graphic_system *system)
{
}
