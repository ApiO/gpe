/*
 * implementation de http://www.gamedev.net/topic/330742-quick-tutorial-variable-width-bitmap-fonts/
 *
 * limites implémentation avec format de l'export de BMFont generator :
 *  - dans la ligne décrivant une [page], le nom du fichier doit être entre guillemet
 *  - hors ligne [info], toutes les veleures des paramétres sont des int sans guillemet
 *  - les fichiers contenant les images de la font doivent être dans le meme repertoire que le fichier *.fnt
 *
 *  => si un des 3 points n'est pas respecté : CRASH !
 *
 */

#ifndef glbmfont_h
#define glbmfont_h

#include <GL/glfw.h>

#define GLBMFONT_CHARS_SIZE   256
#define GLBMFONT_TEX_2D_SIZE  8

typedef struct FVect
{
  GLfloat x, y;
} gpeVect;

 //glbmfont_char holds the information for a single character
 // and precalc the glTexCoord values for each point per char
typedef struct glbmfont_char
{
	int x, y;
	int width, height;
	int xOffset, yOffset;
	int xAdvance;
	int page;
  int chnl;
  gpeVect texCoord[4];
} glbmfont_char;

//glbmfont holds the descriptions that apply to all characters, 
//  as well as the descriptors for every character
typedef struct glbmfont
{
	int lineHeight;
	int base;
	int width, height;
	int pages; 
  int chnl;
	glbmfont_char chars[GLBMFONT_CHARS_SIZE];
  GLuint       tex_2d[GLBMFONT_TEX_2D_SIZE];
} glbmfont;

int  glbmfont_load(void);
int  glbmfont_load(char *file);
void glbmfont_print(char *text, int x, int y);
void glbmfont_free(void);

#endif //glbmfont_h