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
#include "gpr_idlut.h"

#define FS_HASH_SEED  0

typedef gpr_array_t(GLuint) gpe_tex_array;

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
	int     x, y;
	int     width, height;
	int     xOffset, yOffset;
  GLfloat verticies[12];
  GLfloat tex_coord[8];
	int     xAdvance;
	U64     tex_key;
} gpe_font_char;

typedef struct
{
	gpr_hash_t    chars;
	gpe_tex_array tex;
	U32           line_height;
	U32           width, height;
} gpe_bmfont;

typedef struct
{
  GLuint command;
  F64    width;
} gpe_text_line;
typedef gpr_array_t(gpe_text_line) gpe_text_lines;

typedef struct
{
  gpe_text_lines lines;
  U64            font_key;
} gpe_font_string;

typedef struct
{
  gpr_idlut_t texts;
  gpr_hash_t  fonts;
} font_system;

void  font_system_init         (void);
I32   font_system_load_font    (char *key, char *file_path);
U64   font_system_text_init    (void);
U64   font_system_text_init    (char *font_key);
//generates gl command list per lines
void  font_system_text_set     (U64 id, char *t);
void  font_system_text_print   (U64 id, int x, int y, gpe_dock2 dock);
void  font_system_text_destroy (U64 id);
void  font_system_free         (void);

#endif //FONT_SYSTEM_H