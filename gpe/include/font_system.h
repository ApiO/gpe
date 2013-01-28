/*
 * implementation de http://www.gamedev.net/topic/330742-quick-tutorial-variable-width-bitmap-fonts/
 *
 * limites implémentation avec format de l'export de BMFont generator :
 *  1- dans la ligne décrivant une [page], le nom du fichier doit être entre guillemet
 *  2- hors ligne [info], toutes les veleures des paramétres sont des int sans guillemet
 *  3- les fichiers contenant les images de la font doivent être dans le meme repertoire que le fichier *.fnt
 *  4- l'id de [page] commence à 0
 *  5- [page] doivent être dans le fichier fnt dans leur ordre d'ID (sinon on utilisera un hash et non pas array de tex)
 *
 *  => si un des points n'est pas respecté : CRASH !
 *
 */
#ifndef FONT_SYSTEM_H
#define FONT_SYSTEM_H

#include <stdio.h>
#include <GL/glfw.h>
#include "gpr_types.h"
#include "gpr_hash.h"
#include "gpr_array.h"
#include "gpr_idlut.h"

#define FS_KEY_SIZE         256
#define FS_TEXT_SIZE        1024

typedef gpr_array_t(GLuint) tex_array;

typedef enum
{
  dock_top_left2,
  dock_top_right2,
  dock_bottom_left2,
  dock_bottom_right2,
  dock_center2
} gpe_dock2;

typedef struct
{
	int x, y;
	int width, height;
	int xOffset, yOffset;
	int xAdvance;
	U64 tex_key;
} gpe_font_char;

typedef struct
{
  char        file_path[FS_TEXT_SIZE];
	U32         line_height;
	U32         width, height;
	gpr_hash_t  chars;
	tex_array   tex;
} gpe_bmfont;

typedef struct
{
  char    text[FS_TEXT_SIZE];
  char    font_key[FS_KEY_SIZE];
  GLuint  cmd;
} gpe_font_string;

typedef struct
{
  gpr_hash_t  fonts;
  gpr_idlut_t texts;
} font_system;

void  font_system_init        (void);
I32   font_system_load_font   (char *key, char *path);
U64   font_system_text_init   (char *key);
U64   font_system_text_init   (void);
void  font_system_text_print  (U64 i, char *t, int x, int y, gpe_dock2 dock);
void  font_system_text_remove (U64 i);
void  font_system_free        (void);

#endif //FONT_SYSTEM_H