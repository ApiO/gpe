#include "graphic_system.h"

#include <stdio.h>////////TEMP/////////////////////////////////////////////////////////////
#include <GL/glfw.h>

#include "gpr_memory.h"
#include "gpr_array.h"


typedef struct DVect
{
  GLdouble x, y;
} DVect;

typedef struct graphic_node
{
  gpe_graphic   *graphic;
  graphic_node  *next_node;
} graphic_node;

  //--TEMP------------
bool sort_printed = false;////////TEMP/////////////////////////////////////////////////////////////

void _graphc_system_sort (graphic_system *system, void *out);
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
/*
void graphic_system_render_hjhk (graphic_system *system)
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

    _graphic_system_render_quad(graphic, quad, (float)screen_width, 
                                (float)screen_height);

    //init GL
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, graphic->text_id);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                      GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                      GL_LINEAR_MIPMAP_LINEAR );
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.f);
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
    
    //reset GL
    glDisable(GL_TEXTURE_2D);

    //Debug: show text borders
    if(system->border)
    {
      glColor4f(.0f, 1.f, .0f, .5f);
      glBegin(GL_LINE_LOOP);
      {
 	      glVertex2d( quad[TOP_LEFT].x, quad[TOP_LEFT].y );
 	      glVertex2d( quad[BOTTOM_LEFT].x, quad[BOTTOM_LEFT].y );
 	      glVertex2d( quad[BOTTOM_RIGHT].x, quad[BOTTOM_RIGHT].y );
 	      glVertex2d( quad[TOP_RIGHT].x, quad[TOP_RIGHT].y );
      } 
      glEnd();
    }
  }

  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
}
*/
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
  bool loop = true;
  graphic_node gn;
  GLuint *currTex;

  gpr_array_t(graphic_node) arr;
  gpr_array_init(graphic_node, arr, gpr_default_allocator);

  for( i = system->physics_count-1; i >= 0; i-- )
  {
    gn.graphic = &(gpe_graphic)system->table.items[i].value;
    gn.next_node = i == system->physics_count-1 ? 
                    NULL : &gpr_array_item(arr, i+1);

    gpr_array_push_back(graphic_node, arr, gn);
  }

  //sort

  ////////TEMP/////////////////////////////////////////////////////////////
  if(!sort_printed)
  {
    unsigned int j;
    gpe_graphic *graphic;
    printf("---[after sorting]\n");
    for ( j = 0; j < system->physics_count; j++)
    {
      graphic = gpr_array_item(arr, j).graphic;
      printf("tex_id=%d | z=%d\n", graphic->text_id, graphic->z);
    }
    sort_printed = true;
  }
  ////////TEMP/////////////////////////////////////////////////////////////

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

  gn = gpr_array_item(arr, 0);
  currTex = NULL;

  while (loop)
  {
    if (currTex != &gn.graphic->text_id)
    {
      glBindTexture(GL_TEXTURE_2D, gn.graphic->text_id);

      currTex = &gn.graphic->text_id;
    }

    _graphic_system_draw(gn.graphic, (float)screen_width, (float)screen_height, 
                          system->border);
    
    if(gn.next_node == NULL)
    {
      loop = false;
    }
    else
    {
      gn = *gn.next_node;
    }
  }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
  
  gpr_array_destroy(arr);
}

void _graphic_system_draw(gpe_graphic *graphic, float screen_width, 
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