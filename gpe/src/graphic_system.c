#include "graphic_system.h"

#include <stdio.h>////////TEMP/////////////////////////////////////////////////////////////
#include <GL/glfw.h>

#include "gpr_memory.h"
#include "gpr_array.h"

typedef gpr_array_t(gpe_graphic*) graphic_array;

typedef struct DVect
{
  GLdouble x, y;
} DVect;

  //--TEMP------------
bool sort_printed = false;////////TEMP/////////////////////////////////////////////////////////////

void _graphic_system_qsort (graphic_array *out, I32 count);
void _graphic_system_print_array (graphic_array *out, I32 count);
void _graphic_system_draw (gpe_graphic *graphic, float screen_width,
                            float screen_height, bool border);
void _graphic_system_render_quad (gpe_graphic *graphic, DVect *quad, 
                                  float screen_width, float screen_height);

void _graphic_system_render_quad (gpe_graphic *graphic, DVect *q, 
                                  float screen_width, float screen_height)
{
  q[TOP_LEFT].x =     (GLdouble)((float)graphic->x/screen_width) ;
  q[TOP_LEFT].y =     (GLdouble)((float)(graphic->y+graphic->h)/screen_height);
  q[BOTTOM_LEFT].x =  (GLdouble)((float)graphic->x/screen_width);
  q[BOTTOM_LEFT].y =  (GLdouble)((float)graphic->y/screen_height);
  q[BOTTOM_RIGHT].x = (GLdouble)((float)(graphic->x+graphic->w)/screen_width);
  q[BOTTOM_RIGHT].y = (GLdouble)((float)graphic->y/screen_height);
  q[TOP_RIGHT].x =    (GLdouble)((float)(graphic->x+graphic->w)/screen_width);
  q[TOP_RIGHT].y =    (GLdouble)((float)(graphic->y+graphic->h)/screen_height);
}

void graphic_system_init (graphic_system *system, U32 object_count)
{
  gpr_idlut_init(gpe_graphic, &system->table, gpr_default_allocator, 
                  object_count);
  system->physics_count = 0;
  system->capacity = object_count;
  system->border = true;
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

gpe_graphic* graphic_system_lookup (graphic_system *system, U32 graphic_id)
{
  return gpr_idlut_lookup(gpe_graphic, &system->table, graphic_id);
}

void graphic_system_free (graphic_system *system)
{
  gpr_idlut_destroy(gpe_graphic, &system->table);
}

void graphic_system_render (graphic_system *system)
{
  int i;
  int screen_width, screen_height;
  GLuint *currTex;
  gpe_graphic *graphic;

  if (system->physics_count == 0) return;

  graphic_array arr;
  gpr_array_init(gpe_graphic*, arr, gpr_default_allocator);
  
  for( i = 0; i < system->physics_count; i++ )
  {
    graphic = &system->table.items[i].value;
    gpr_array_push_back(gpe_graphic*, arr, graphic);
  }

  //sort
  _graphic_system_print_array(&arr, system->physics_count);
  _graphic_system_qsort(&arr, system->physics_count);
  _graphic_system_print_array(&arr, system->physics_count);

	glfwGetWindowSize(&screen_width, &screen_height);

  //init GL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                    GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                    GL_LINEAR_MIPMAP_LINEAR );
                    
  currTex = NULL;

  for ( i = 0; i < system->physics_count; i++)
  {
    graphic = gpr_array_item(arr, i);
    if (currTex != &graphic->text_id)
    {
      glBindTexture(GL_TEXTURE_2D, graphic->text_id);
      currTex = &graphic->text_id;
    }

    _graphic_system_draw(graphic, (float)screen_width, (float)screen_height, 
                          system->border);
  }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
  
  gpr_array_destroy(arr);
}


void _graphic_system_draw (gpe_graphic *graphic, float screen_width, 
                           float screen_height, bool border)
{
  DVect quad[4];
  _graphic_system_render_quad(graphic, quad, screen_width, screen_height);

  glColor4f(1.0f, 1.0f, 1.0f, 1.f);
  
  glEnable(GL_TEXTURE_2D);

  //rendu de l'élément
  glBegin(GL_QUADS);
  {
 	  glTexCoord2f( graphic->texCoord[TOP_LEFT].x,
                  graphic->texCoord[TOP_LEFT].y );
 	  glVertex2d(	  quad[TOP_LEFT].x, 
                  quad[TOP_LEFT].y );
      
 	  glTexCoord2f(	graphic->texCoord[BOTTOM_LEFT].x, 
                  graphic->texCoord[BOTTOM_LEFT].y );
 	  glVertex2d(	  quad[BOTTOM_LEFT].x, 
                  quad[BOTTOM_LEFT].y );
      
 	  glTexCoord2f(	graphic->texCoord[BOTTOM_RIGHT].x, 
                  graphic->texCoord[BOTTOM_RIGHT].y) ;
 	  glVertex2d(	  quad[BOTTOM_RIGHT].x, 
                  quad[BOTTOM_RIGHT].y );
 
 	  glTexCoord2f(	graphic->texCoord[TOP_RIGHT].x, 
                  graphic->texCoord[TOP_RIGHT].y) ;
 	  glVertex2d(	  quad[TOP_RIGHT].x, 
                  quad[TOP_RIGHT].y );
	} 
  glEnd();

  glDisable(GL_TEXTURE_2D);
    
  //Debug: show tex borders
  if(border)
  {
    glColor4f(.0f, 1.f, .0f, .5f);
    glBegin(GL_LINE_LOOP);
    {
 	    glVertex2d(	  quad[TOP_LEFT].x, 
                    quad[TOP_LEFT].y );
 	    glVertex2d(	  quad[BOTTOM_LEFT].x, 
                    quad[BOTTOM_LEFT].y );
 	    glVertex2d(	  quad[BOTTOM_RIGHT].x, 
                    quad[BOTTOM_RIGHT].y );
 	    glVertex2d(	  quad[TOP_RIGHT].x, 
                    quad[TOP_RIGHT].y );
    } 
    glEnd();
  }
}

void _graphic_system_print_array (graphic_array *out, I32 count)
{
  gpe_graphic *graphic;
  if(!sort_printed)
  {
    unsigned int j;
    printf("---[after sorting]\n");
    for ( j = 0; j < count; j++)
    {
      graphic = out->data[j];
      printf("z = %d | tex_id = %d\n", graphic->z, graphic->text_id);
    }
    sort_printed = true;
  }
}

void _graphic_system_qsort (graphic_array *out, I32 count)
{
}