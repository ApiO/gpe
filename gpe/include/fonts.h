#ifndef fonts_h
#define fonts_h

/*
 * implementation de http://www.gamedev.net/topic/330742-quick-tutorial-variable-width-bitmap-fonts/
 *
 * limites implémentation avec format de l'export de BMFont generator
 *  - dans la ligne décrivant une page, le nom du fichier doit être entre guillemet
 *  - hors ligne info, toutes les veleures des paramétres sont des int sans guillemet
 *  - les fichiers contenant les images de la font doivent être dans le meme repertoire que le fichier fnt
 *
 *  => si un des 3 pont n'est pas respecté : CRASH !
 *
 */

 //CharDescriptor holds the information for a single character
typedef struct charDescriptor
{
	int x, y;
	int Width, Height;
	int XOffset, YOffset;
	int XAdvance;
	int Page;
  int Chnl;
} charDescriptor;

//Charset holds the descriptions that apply to all characters, as well as the descriptors for every character
typedef struct charset
{
	int LineHeight;
	int Base;
	int Width, Height;
	int Pages;
  int Chnl;
	charDescriptor Chars[256];
  unsigned int tex_2d[8];
} charset;

int  fonts_load(void);
int  fonts_load(char *file);
void fonts_print(char *text, int x, int y);
void fonts_free(void);

#endif //fonts_h