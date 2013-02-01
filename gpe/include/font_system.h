/*
 * implementation de http://www.gamedev.net/topic/330742-quick-tutorial-variable-width-bitmap-fonts/
 *
 * limites implémentation avec format de l'export de BMFont generator :
 *  1- dans la ligne décrivant une [page], le nom du fichier doit être entre guillemet
 *  2- hors ligne [info], toutes les veleures des paramétres sont des int sans guillemet
 *  3- les fichiers contenant les images de la font doivent être dans le meme repertoire que le fichier *.fnt
 *
 *  => si un des points n'est pas respecté : CRASH !
 *
 */
#ifndef FONT_SYSTEM_H
#define FONT_SYSTEM_H

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>
#include <stdio.h>
#include "gpr_types.h"
#include "gpr_hash.h"
#include "gpr_array.h"
#include "gpr_idlut.h"

static char *FSYS_DEFAULT_FONT_NAME = "consolas.24";

typedef enum
{
  DOCK_TEXT_TOP_LEFT,
  DOCK_TEXT_TOP_RIGHT,
  DOCK_TEXT_BOTTOM_LEFT,
  DOCK_TEXT_BOTTOM_RIGHT,
  DOCK_TEXT_CENTER
} gpe_text_dock;

typedef enum
{
  ALIGN_TEXT_LEFT,
  ALIGN_TEXT_RIGHT,
  ALIGN_TEXT_CENTER
} gpe_text_align;

typedef struct
{
	U64 tex_key;
	I32 x, y, h, w;
  I32 x_off, y_off;
	I32 xAdvance;
  GLfloat tex_coord[8];
} gpe_font_char;

typedef struct
{
	gpr_hash_t chars;
	gpr_hash_t tex;
	U32        line_height;
	U32        width, height;
} gpe_bmfont;

typedef struct
{
  U64      font_key;
  GLuint   cmd_id;
  U32      width;
  U32      height;
} gpe_text;

typedef struct
{
  gpr_idlut_t texts;
  gpr_hash_t  fonts;
  gpr_allocator_t *a;
} font_system;

void  font_system_init         (gpr_allocator_t *a);
I32   font_system_load_font    (char *key, char *file_path);
U64   font_system_text_init    (char *font_key);
void  font_system_text_set     (U64 id, wchar_t *t, gpe_text_align align);
void  font_system_text_print   (U64 id, F32 x, F32 y, gpe_text_dock dock, F32 screen_height, F32 screen_width);
void  font_system_text_destroy (U64 id);
void  font_system_free         (void);

#endif //FONT_SYSTEM_H