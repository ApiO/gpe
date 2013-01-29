#include "font_system.h"

#include <string.h>
#include <stdlib.h>
#include <SOIL\SOIL.h>
#include <assert.h>
#include "gpr_murmur_hash.h"

char *default_font = "QuicksandBook";
char *default_font_path = "..\\..\\src\\ressources\\QuicksandBook.fnt";

static I32   _indexof (char *str1, char *str2);
static I32   _last_indexof (char *str1, char str2);
static char *_substring (int start, int stop, char *src);
static char *_strappend (char *str1, char *str2);
I32   _fsys_parse_common   (gpe_bmfont *f, char *line);
I32   _fsys_parse_page     (gpe_bmfont *f, char *line, char *path);
I32   _fsys_parse_char     (gpe_bmfont *f, char *line);
I32   _fsys_load_font      (gpe_bmfont *f, char *file_path);
I32   _fsys_get_key_value  (char *line, char *key);
char *_fsys_get_font_path  (char *filepath);
char *_fsys_get_path_value (char *line, char *key);

static font_system _fontsystem;

/*
 *  PUBLIC
 */

void font_system_init()
{
  gpr_hash_init(gpe_bmfont, &_fontsystem.fonts, gpr_default_allocator);
  gpr_idlut_init(gpe_font_string, &_fontsystem.texts, gpr_default_allocator);

  if (font_system_load_font(default_font, default_font_path) < 0)
    exit( EXIT_FAILURE );
}

I32 font_system_load_font (char *key, char *file_path)
{
  gpe_bmfont f;
   
  gpr_hash_init(gpe_font_char, &f.chars, gpr_default_allocator);
  gpr_array_init(GLuint, &f.tex, gpr_default_allocator);

  _fsys_load_font(&f, file_path);

  gpr_hash_set(gpe_bmfont, &_fontsystem.fonts, gpr_murmur_hash_64(key, strlen(key), FS_HASH_SEED), &f);

  return 0;
}

U64  font_system_text_init (void)
{
  return font_system_text_init(default_font);
}

U64  font_system_text_init (char *font_key)
{
  gpe_font_string s;
  s.font_key = gpr_murmur_hash_64(font_key, strlen(font_key), FS_HASH_SEED);
  gpr_array_init(gpe_text_line, &s.lines, gpr_default_allocator);
  return gpr_idlut_add(gpe_font_string, &_fontsystem.texts, &s);
}


void  font_system_text_set (U64 id, char *t)
{
  /*
  pour chaques lignes dans le texte
    on ajoute un gpe_text_line au gpe_font_string correspondant à l'id
      on génére la liste de commande et renseigne le gpe_text_line.command
  */
}

void font_system_text_print (U64 id, int x, int y, gpe_dock2 dock)
{
  //TODO
  gpe_font_string *o = gpr_idlut_lookup(gpe_font_string, &_fontsystem.texts, id);

  //init gl 
  //pour chaques lignes
  //  transfo en prenant le docking en compte
  //  glCallList(o->cmd);
  //reset gl
}

void font_system_text_destroy (U64 id)
{
  U32 i;
  gpe_font_string *fs = gpr_idlut_lookup(gpe_font_string, &_fontsystem.texts, id);
  
  for (i=0; i < fs->lines.size; i++)
  {
    GLuint cid = gpr_array_item(&fs->lines, i).command;
    if(cid > 0) glDeleteLists(cid, 1);
  }
  gpr_array_destroy(&fs->lines);
  
  gpr_idlut_remove(gpe_font_string, &_fontsystem.texts, id);
}

void font_system_free ()
{
  U32 i, j;

  //clean texts
  for (i=0; i<_fontsystem.texts.num_items; i++)
  {
    gpe_text_lines *tl = &(gpr_idlut_begin(gpe_font_string, &_fontsystem.texts) + i)->lines;
    for (j=0; j < tl->size; j++)
      glDeleteLists(gpr_array_item(tl, j).command, 1);
    gpr_array_destroy(tl);
  }
  gpr_idlut_destroy(gpe_font_string, &_fontsystem.texts);

  //clean fonts
  for (i=0; i<_fontsystem.fonts.num_values; i++)
  {
    gpe_bmfont *f = gpr_hash_begin(gpe_bmfont, &_fontsystem.fonts) + i;
    //clean chars
    gpr_hash_destroy(gpe_font_char, &f->chars);
    //clean tex
    for (j=0; j<f->tex.size; j++)
      glDeleteTextures(1, &gpr_array_item(&f->tex, j));
    gpr_array_destroy(&f->tex);
  }
  gpr_hash_destroy(gpe_bmfont, &_fontsystem.fonts);
}


/*
 *  PRIVATE
 */

I32 _fsys_parse_common (gpe_bmfont *f, char *line)
{
  f->line_height = _fsys_get_key_value(line, "lineHeight=");
  f->width =       _fsys_get_key_value(line, "scaleW=");
  f->height =      _fsys_get_key_value(line, "scaleH=");
  return 1;
}

I32 _fsys_parse_page (gpe_bmfont *f, char *line, char *path)
{
  U32 id;
  char *filename;
  char *imagepath;
  GLuint tex_id;

  id =        _fsys_get_key_value (line, "id=");
  filename =  _fsys_get_path_value (line, "file=");
  imagepath = _strappend(path, filename);

  tex_id = SOIL_load_OGL_texture (
    imagepath, 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  if( tex_id == 0 )
  {
    printf("Couldn't open file \"%s\" for reading.\n", imagepath);
    free(filename);
    free(imagepath);
    return -1;
  }

  gpr_array_push_back(GLuint, &f->tex, tex_id);
  assert(gpr_array_item(&f->tex, id) == tex_id); //contrôle du point "5" cf ".h"
    
  free(filename);
  free(imagepath);
  
  return 1;
}

I32 _fsys_parse_char (gpe_bmfont *f, char *line)
{
  return 1;
}

I32 _fsys_load_font (gpe_bmfont *f, char *file_path)
{ 
  char line_buffer[BUFSIZ];
  char *path;
  FILE *file = fopen(file_path, "r");
  I32 state = 1;

  if (!file) {
      printf("Couldn't open file \"%s\" for reading.\n", file_path);
      return -1;
  }

  path = _fsys_get_font_path(file_path);
  while (fgets(line_buffer, sizeof(line_buffer), file) && state == 1)
  {
    if (_indexof(line_buffer, "common ") > -1) 
    {   
      state = _fsys_parse_common(f, line_buffer);
    }
    else if (_indexof(line_buffer, "page ") > -1) 
    {   
      state = _fsys_parse_page(f, line_buffer, path);
    }
    else if (_indexof(line_buffer, "char ") > -1) 
    {   
      state = _fsys_parse_char(f, line_buffer);
    }
  }

  fclose(file);
  free(path);
  
  return state;
}

I32 _fsys_get_key_value (char *line, char *key)
{
  U32 value;
  char *result;
  char *tmp;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = _indexof(line, key);
  value_start_index = key_start_index + strlen(key);

  tmp = _substring(value_start_index, strlen(line)-value_start_index, line);
  
  value_end_index = _indexof(tmp, " ");
  if (value_end_index < 0) value_end_index = _indexof(tmp, "\0");

  result = _substring(0, value_end_index, tmp);
  
  value = atoi(result);

  free(result);
  free(tmp);

  return value;
}

char *_fsys_get_font_path (char *filepath)
{
  char *path;
  int index;
  index = _last_indexof(filepath, '\\');

  if(index != NULL && index > 0)
    path = _substring(0, index+1, filepath);
  else
  {
    path = (char *) malloc(strlen(filepath));
    strcpy(path, path);
  }
  return path;
}

static I32 _last_indexof (char *str, char chr)
{
  U32 i, len;
  len = strlen(str);
  for (i=len-1; i>=0; i--)  if(str[i] == chr) return i;
  return NULL;
}

static char *_substring (int start, int stop, char *src)
{
  //char out[BUFSIZ];
  char *out = (char*)malloc(stop-start+1);
  int count = stop - start;
  sprintf(out, "%.*s", count, src + start);
  return out;
}

I32 _indexof (char *str1, char *str2)
{
  char *tmp = strstr(str1, str2);
  return tmp - str1;
}

char *_fsys_get_path_value (char *line, char *key)
{
  char *tmp;
  char *result;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = _indexof(line, key);
  value_start_index = key_start_index + strlen(key) + 1;

  tmp = _substring(value_start_index, strlen(line), line);
  
  value_end_index = _indexof(tmp, "\"");

  result = _substring(0, value_end_index, tmp);
  
  free(tmp);
  return result;
}

static char *_strappend (char *str1, char *str2)
{
  char *s = (char *)malloc((strlen(str1) + strlen(str2) +1)*sizeof(char));
  sprintf(s, "%s%s", str1, str2);
  return s;
}