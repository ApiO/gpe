#include "gl_renderer.h"

#include <GL/glfw.h>

#define TOP_LEFT      0
#define BOTTOM_LEFT   1
#define BOTTOM_RIGHT  2
#define TOP_RIGHT     3

void _gl_renderer_draw_graphic(gpe_gl_graphic *graphic, bool debug);


void gl_renderer_add(gpe_gl_renderer *renderer, gpe_gl_graphic graphic)
{
  int screen_width, screen_height;

  if(renderer->init)
  {
    gpr_array_init(gpe_gl_graphic, &renderer->graphics, gpr_default_allocator);
    renderer->init = false;
  }

	glfwGetWindowSize(&screen_width, &screen_height);
  
  graphic.texCoord[TOP_LEFT].x = 0;
  graphic.texCoord[TOP_LEFT].y = 0;
  graphic.texCoord[BOTTOM_LEFT].x = 0;
  graphic.texCoord[BOTTOM_LEFT].y = 1;
  graphic.texCoord[BOTTOM_RIGHT].x = 1;
  graphic.texCoord[BOTTOM_RIGHT].y = 1;
  graphic.texCoord[TOP_RIGHT].x = 1;
  graphic.texCoord[TOP_RIGHT].y = 0; 

  graphic.quad[TOP_LEFT].x =     (GLdouble)((float)graphic.x/screen_width) ;
  graphic.quad[TOP_LEFT].y =     (GLdouble)((float)(graphic.y+graphic.h)/screen_height);
  graphic.quad[BOTTOM_LEFT].x =  (GLdouble)((float)graphic.x/screen_width);
  graphic.quad[BOTTOM_LEFT].y =  (GLdouble)((float)graphic.y/screen_height);
  graphic.quad[BOTTOM_RIGHT].x = (GLdouble)((float)(graphic.x+graphic.w)/screen_width);
  graphic.quad[BOTTOM_RIGHT].y = (GLdouble)((float)graphic.y/screen_height);
  graphic.quad[TOP_RIGHT].x =    (GLdouble)((float)(graphic.x+graphic.w)/screen_width);
  graphic.quad[TOP_RIGHT].y =    (GLdouble)((float)(graphic.y+graphic.h)/screen_height);
  
  gpr_array_push_back(gpe_gl_graphic, &renderer->graphics, graphic);
}

void gl_renderer_draw(gpe_gl_renderer *renderer)
{
  U32 i;
  GLuint *curr_tex;
  gpe_gl_graphic *graphic;

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
  
  curr_tex = NULL;
  i = renderer->graphics.size;
  do 
  {
    i -= 1;
    graphic = &gpr_array_item(&renderer->graphics, i);
    if (curr_tex != &graphic->tex_id)
    {
      glBindTexture(GL_TEXTURE_2D, graphic->tex_id);
      curr_tex = &graphic->tex_id;
    }
    _gl_renderer_draw_graphic(graphic, renderer->debug);
  } while(i!=0);
          
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);

  gpr_array_destroy(&renderer->graphics);
  renderer->init = true;
}

void _gl_renderer_draw_graphic(gpe_gl_graphic *graphic, bool debug)
{
  glColor4f(1.0f, 1.0f, 1.0f, 1.f);
  
  glEnable(GL_TEXTURE_2D);

  //rendu de l'élément
  glBegin(GL_QUADS);
  {
 	  glTexCoord2f( graphic->texCoord[TOP_LEFT].x,
                  graphic->texCoord[TOP_LEFT].y );
 	  glVertex2d( graphic->quad[TOP_LEFT].x, graphic->quad[TOP_LEFT].y );
      
 	  glTexCoord2f(	graphic->texCoord[BOTTOM_LEFT].x, 
                  graphic->texCoord[BOTTOM_LEFT].y );
 	  glVertex2d( graphic->quad[BOTTOM_LEFT].x, graphic->quad[BOTTOM_LEFT].y );
      
 	  glTexCoord2f(	graphic->texCoord[BOTTOM_RIGHT].x, 
                  graphic->texCoord[BOTTOM_RIGHT].y) ;
 	  glVertex2d( graphic->quad[BOTTOM_RIGHT].x, graphic->quad[BOTTOM_RIGHT].y );
 
 	  glTexCoord2f(	graphic->texCoord[TOP_RIGHT].x, 
                  graphic->texCoord[TOP_RIGHT].y) ;
 	  glVertex2d( graphic->quad[TOP_RIGHT].x, graphic->quad[TOP_RIGHT].y );
	} 
  glEnd();

  glDisable(GL_TEXTURE_2D);
    /*
  //Debug: show tex borders
  if(debug)
  {
    glColor4f(.0f, 1.f, .0f, .5f);
    glBegin(GL_LINE_LOOP);
    {
 	    glVertex2d( graphic->quad[TOP_LEFT].x, graphic->quad[TOP_LEFT].y );
 	    glVertex2d( graphic->quad[BOTTOM_LEFT].x, graphic->quad[BOTTOM_LEFT].y );
 	    glVertex2d( graphic->quad[BOTTOM_RIGHT].x, graphic->quad[BOTTOM_RIGHT].y );
 	    glVertex2d( graphic->quad[TOP_RIGHT].x, graphic->quad[TOP_RIGHT].y );
    } 
    glEnd();
  }
    */
}