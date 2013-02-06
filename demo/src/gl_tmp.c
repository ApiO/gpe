#include "gl_tmp.h"


#include "window_manager.h"
#include <SOIL\SOIL.h>

#define GL_GLEXT_PROTOTYPES

#define HEIGHT      600
#define WIDTH       800



void GL_TMP_H_FOO()
{
  window_manager w;
  GLuint tex_id;
  F32 verticies[8] = {0,0, 0,128, 128,128, 128,0 };
  F32 tex_coord[8] = {0,0, 0,1, 1,1, 1,0};

  gpr_memory_init(410241024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_axes = 1;
  w.display_fps = 1;


  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  //glGenBuffers(
  

  while(w.running)
  {
    window_manager_clear(&w);
      

      
    window_manager_swapBuffers(&w);
  }
     
  glDeleteTextures(1, &tex_id);
  window_manager_free(&w);
  gpr_memory_shutdown();
}