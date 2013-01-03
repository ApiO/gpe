#include "graphic_system.h"

#include <GL/glfw.h>

#include "gpr_memory.h"
#include "gpr_array.h"

typedef struct DVect
{
  GLdouble x, y;
} DVect;

void _graphic_system_render_quads (gpe_graphic *graphic, DVect *quad, float screen_width, float screen_height);


void _graphic_system_render_quads (gpe_graphic *graphic, DVect *quad, float screen_width, float screen_height)
{
  quad[TEX_TOP_LEFT].x =     (GLdouble) ((float)graphic->x / screen_width) ;
  quad[TEX_TOP_LEFT].y =     (GLdouble) ((float)(graphic->y + graphic->h)/screen_height);
  quad[TEX_BOTTOM_LEFT].x =  (GLdouble) ((float)graphic->x / screen_width);
  quad[TEX_BOTTOM_LEFT].y =  (GLdouble) ((float)graphic->y / screen_height);
  quad[TEX_BOTTOM_RIGHT].x = (GLdouble) ((float)(graphic->x + graphic->w)/screen_width);
  quad[TEX_BOTTOM_RIGHT].y = (GLdouble) ((float)graphic->y / screen_height);
  quad[TEX_TOP_RIGHT].x =    (GLdouble) ((float)(graphic->x + graphic->w)/screen_width);
  quad[TEX_TOP_RIGHT].y =    (GLdouble) ((float)(graphic->y + graphic->h)/screen_height);
}

void graphic_system_init (graphic_system *system, U32 object_count)
{
  gpr_idlut_init(gpe_graphic, &system->table, gpr_default_allocator, object_count);
  system->physics_count = 0;
  system->capacity = object_count;
}

//return idlut id of the tex within the system
U32 graphic_system_add (graphic_system *system, GLuint tex_id)
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
  unsigned int i;
  gpe_graphic *graphic;
  DVect quad[4];
  int screen_width, screen_height;
  
	glfwGetWindowSize(&screen_width, &screen_height);
  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for(i=0; i < system->physics_count; i++)
  {
    graphic = &(gpe_graphic)system->table.items[i].value;

    _graphic_system_render_quads(graphic, quad, (float)screen_width, (float)screen_height);

    //init GL
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, graphic->text_id);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.f);
    //rendu de l'élément
    glBegin(GL_QUADS);
    {
 	    glTexCoord2f( graphic->texCoord[TEX_TOP_LEFT].x,
                    graphic->texCoord[TEX_TOP_LEFT].y );
 	    glVertex2d(	  quad[TEX_TOP_LEFT].x, 
                    quad[TEX_TOP_LEFT].y );
      
 	    glTexCoord2f(	graphic->texCoord[TEX_BOTTOM_LEFT].x, 
                    graphic->texCoord[TEX_BOTTOM_LEFT].y );
 	    glVertex2d(	  quad[TEX_BOTTOM_LEFT].x, 
                    quad[TEX_BOTTOM_LEFT].y );
      
 	    glTexCoord2f(	graphic->texCoord[TEX_BOTTOM_RIGHT].x, 
                    graphic->texCoord[TEX_BOTTOM_RIGHT].y) ;
 	    glVertex2d(	  quad[TEX_BOTTOM_RIGHT].x, 
                    quad[TEX_BOTTOM_RIGHT].y );
 
 	    glTexCoord2f(	graphic->texCoord[TEX_TOP_RIGHT].x, 
                    graphic->texCoord[TEX_TOP_RIGHT].y) ;
 	    glVertex2d(	  quad[TEX_TOP_RIGHT].x, 
                    quad[TEX_TOP_RIGHT].y );
	  } 
    glEnd();
    
    //reset GL
    glDisable(GL_TEXTURE_2D);

    glColor4f(.0f, 1.f, .0f, .5f);
    glBegin(GL_LINE_LOOP);
    {
 	    glVertex2d(	  quad[TEX_TOP_LEFT].x, 
                    quad[TEX_TOP_LEFT].y );
 	    glVertex2d(	  quad[TEX_BOTTOM_LEFT].x, 
                    quad[TEX_BOTTOM_LEFT].y );
 	    glVertex2d(	  quad[TEX_BOTTOM_RIGHT].x, 
                    quad[TEX_BOTTOM_RIGHT].y );
 	    glVertex2d(	  quad[TEX_TOP_RIGHT].x, 
                    quad[TEX_TOP_RIGHT].y );
    } 
    glEnd();
  }

  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
}

gpe_graphic* graphic_system_lookup (graphic_system *system, U32 graphic_id)
{
  return gpr_idlut_lookup(gpe_graphic, &system->table, graphic_id);
}

void graphic_system_free (graphic_system *system)
{
  gpr_idlut_destroy(gpe_graphic, &system->table);
}
