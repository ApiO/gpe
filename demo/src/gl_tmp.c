#include "gl_tmp.h"

#include <stdio.h>
#include <SOIL\SOIL.h>
#include "window_manager.h"

#define HEIGHT  600
#define WIDTH   800

void GL_TMP_H_FOO()
{
  window_manager w;
  U32 vbo[2];
  //U32 vao;
  U32 tex_id;
  F32 verticies[12] = { 0,0, 0,128, 128,128, 
                        128,128, 128,0, 0,0 };
  F32 tex_coord[12] = { 0,0, 0,1, 1,1, 
                        1,1, 1,0, 0,0};

  gpr_memory_init(4*1024*1024);

  window_manager_init(&w, "simple gl rendering test", HEIGHT, WIDTH);
  w.display_axes = 1;
  w.display_fps = 1;

  tex_id = SOIL_load_OGL_texture (
    "..\\..\\src\\ressources\\patate2.png", 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS );

  /*
	glGenVertexArrays(1, &vao); // Créer le VAO
  glBindVertexArray(vao);     // Lier le VAO pour l'utiliser
  //*/
	// Generate And Bind The Vertex Buffer
	glGenBuffers( 1, &vbo[0] );							                                              // Get A Valid Name
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );			                                      // Bind The Buffer
  glBufferData( GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);  // Load The Data
	glGenBuffers( 1, &vbo[1] );
  glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
  /*
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); // Définir le pointeur d'attributs des sommets
	glEnableVertexAttribArray(0); // Désactiver le VAO
	glBindVertexArray(0); // Désactiver le VBO
  //*/
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  while(w.running)
  {
    window_manager_clear(&w);
          
   glPushMatrix();
    {
      glTranslatef(50, 50, 0.f);
      //*
      glColor3f(1.f, 1.f, 1.f);
		  glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
		  glVertexPointer( 2, GL_FLOAT, 0, 0 );		// Set The Vertex Pointer To The Vertex Buffer
		  glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
      glBindTexture( GL_TEXTURE_2D, tex_id);
		  glTexCoordPointer( 2, GL_FLOAT, 0, 0 );	
	    glDrawArrays( GL_TRIANGLES, 0, 6 );
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    } glPopMatrix();
    //*/
    /*
    glColor3f(1.f, 1.f, 1.f);
    glBindTexture( GL_TEXTURE_2D, tex_id);
    glBindVertexArray(vao); // Lier le VAO
    glDrawArrays(GL_TRIANGLES, 0, 6); // Dessiner le carré
    glBindVertexArray(0); // Délier le VAO 
    //*/
    window_manager_swapBuffers(&w);
  }

  
  //glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(2, vbo);
  glDeleteTextures(1, &tex_id);

  window_manager_free(&w);
  gpr_memory_shutdown();
}