#include "font_system.h"

#include <string.h>
#include <SOIL\SOIL.h>
#include <assert.h>
#include "gpr_murmur_hash.h"
#include "gpr_tmp_allocator.h"
#include "gpr_sort.h"

typedef struct
{
  wchar_t c;
  U64     tex_key;
  U32     x, line_index;
} fsys_char_pos_t;
typedef gpr_array_t(fsys_char_pos_t) fsys_poses;

typedef gpr_array_t(F32) fsys_lines_padx;

typedef struct
{
	U64 tex_key;
  U32 first, count;
} fsys_char_group_t;
typedef gpr_array_t(fsys_char_group_t) fsys_char_groups;

static I32   _indexof (char *str1, char *str2);
static I32   _last_indexof (char *str1, char str2);
static char *_substring (int start, int stop, char *src, gpr_allocator_t *a);
static char *_strappend (char *str1, char *str2, gpr_allocator_t *a);
I32   _fsys_parse_common      (gpe_bmfont *f, char *line);
I32   _fsys_parse_page        (gpe_bmfont *f, char *line, char *path);
I32   _fsys_parse_char        (gpe_bmfont *f, char *line);
I32   _fsys_load_font         (gpe_bmfont *f, char *file_path);
I32   _fsys_get_key_value_int (char *line, char *key);
char *_fsys_get_key_value     (char *line, char *key);
char *_fsys_get_font_path     (char *filepath, gpr_allocator_t *a);
char *_fsys_get_path_value    (char *line, char *key, gpr_allocator_t *a);
void _fsys_set_lines_pad  (fsys_lines_padx *lines, U32 max, gpe_text_align align);
void _fsys_set_poses      (gpe_text *text, wchar_t *t, gpe_bmfont *font, fsys_poses *poses,
                            fsys_lines_padx *lines, gpr_allocator_t *a);
void _fsys_create_command (gpe_text *text, gpe_bmfont *font, fsys_poses *poses, 
                            fsys_lines_padx *lines, gpr_allocator_t *a);

#define FSYS_H_TEX_SEED    0
#define FSYS_H_FONT_SEED   0
#define _poses_comp(a, b) ((a).tex_key < (b).tex_key)

static char *default_font_path = "..\\..\\src\\ressources\\consolas.24\\consolas.24.fnt";
static font_system _fontsystem;
static wchar_t missing_char_key = L'_';

/*
 *  PUBLIC
 */

void font_system_init(gpr_allocator_t *a)
{
  _fontsystem.a = a;
  gpr_hash_init(gpe_bmfont, &_fontsystem.fonts, a);
  gpr_idlut_init(gpe_text, &_fontsystem.texts, a);

  if (font_system_load_font(FSYS_DEFAULT_FONT_NAME, default_font_path) < 0)
    exit( EXIT_FAILURE );
}

I32 font_system_load_font (char *key, char *file_path)
{
  gpe_bmfont f;
   
  gpr_hash_init(gpe_font_char, &f.chars, gpr_default_allocator);
  gpr_hash_init(GLuint, &f.tex, gpr_default_allocator);

  _fsys_load_font(&f, file_path);

  gpr_hash_set(gpe_bmfont, &_fontsystem.fonts, gpr_murmur_hash_64(key, strlen(key), FSYS_H_FONT_SEED), &f);

  return 0;
}

U64  font_system_text_init (char *font_key)
{
  gpe_text s;
  s.font_key = gpr_murmur_hash_64(font_key, strlen(font_key), FSYS_H_FONT_SEED);
  s.cmd_id = 0;
  s.cmd_id = glGenLists(1);
  return gpr_idlut_add(gpe_text, &_fontsystem.texts, &s);
}

void  font_system_text_set (U64 id, wchar_t *t, gpe_text_align align)
{
  gpe_text   *text;
  gpe_bmfont *font; 
  fsys_poses  poses;
  fsys_lines_padx  lines;
  gpr_tmp_allocator_2048_t  ta;
  gpr_allocator_t          *a = (gpr_allocator_t*)&ta;
  
  gpr_tmp_allocator_init(a, 2048);
  
  text = gpr_idlut_lookup(gpe_text, &_fontsystem.texts, id);
  font = gpr_hash_get(gpe_bmfont, &_fontsystem.fonts, text->font_key);
  
  _fsys_set_poses (text, t, font, &poses, &lines, a);
  _fsys_set_lines_pad (&lines, text->width, align);
  gpr_mergesort(poses.data, poses.size, fsys_char_pos_t, _poses_comp, a);
  _fsys_create_command(text, font, &poses, &lines, a);
  
  gpr_array_destroy(&poses);
  gpr_array_destroy(&lines);
  gpr_tmp_allocator_destroy(a);
}

void _fsys_set_poses (gpe_text *text, wchar_t *t, gpe_bmfont *font, fsys_poses *poses, fsys_lines_padx *lines, gpr_allocator_t *a)
{
  U32 i, len;
  U32 max_width, curr_width, line_index;
  len = wcslen(t);

  gpr_array_init(fsys_char_pos_t, poses, a);
  gpr_array_reserve(fsys_char_pos_t, poses, len);

  gpr_array_init(F32, lines, a);
  {
    U32 line_count = 1;
    for(i=0; i<len; i++)
      if(t[i] == '\n') line_count+=1;
    gpr_array_reserve(F32, lines, line_count*sizeof(F64));
  }

  curr_width=0; max_width=0; line_index=0;
  for(i=0; i<=len;i++)
  {
    if (t[i] == '\n' || i == len)
    {
      gpr_array_push_back(F32, lines, (F32)curr_width);
      line_index += 1;
      if(max_width<curr_width) max_width = curr_width;
      curr_width = 0;
      continue;
    }
    {
      gpe_font_char *chr;
      fsys_char_pos_t pos;
      wchar_t chr_key;
        
      if(gpr_hash_has(gpe_font_char, &font->chars, (U64)t[i]))
      {
        chr = gpr_hash_get(gpe_font_char, &font->chars, (U64)t[i]);
        chr_key = t[i];
      }
      else
      {
        chr = gpr_hash_get(gpe_font_char, &font->chars, (U64)missing_char_key);
        chr_key = missing_char_key;
      }
        
      pos.c = chr_key;
      pos.tex_key = chr->tex_key;
      pos.x = curr_width;
      pos.line_index = line_index;
      gpr_array_push_back(fsys_char_pos_t, poses, pos);
      curr_width += chr->xAdvance;
    }
  }
  text->width = max_width;
  text->height = font->line_height*line_index;
}

void _fsys_set_lines_pad (fsys_lines_padx *lines, U32 max, gpe_text_align align)
{
  U32 i;
  F32 pad_value;
  switch(align)
  {
    case ALIGN_TEXT_LEFT: 
      pad_value = 0.f;
      break;
    case ALIGN_TEXT_RIGHT: 
      pad_value = 1.f;
      break;
    case ALIGN_TEXT_CENTER: 
      pad_value = .5f;
      break;
  }
  
  for(i=0; i<lines->size;i++)
  {
    F32 *pad_x = &gpr_array_item(lines, i);
    *pad_x = ((max - (*pad_x)) * pad_value);
  }

}

void _fsys_create_command(gpe_text *text, gpe_bmfont *font, fsys_poses *poses, fsys_lines_padx *lines, gpr_allocator_t *a)
{
  U32 i;
  fsys_char_groups  char_groups;
  GLfloat *verticies, *tex_coord;

  //init chars groups
  gpr_array_init(fsys_char_group_t, &char_groups, a);
  gpr_array_reserve(fsys_char_group_t, &char_groups, font->tex.num_values);
  {
    U64 tex_key = gpr_array_begin(poses)->tex_key;
    U32 start_index = 0;
    
    for(i=1; i<=poses->size; i++)
    {
      if(gpr_array_item(poses, i).tex_key == tex_key && i<poses->size) continue;
      {
        fsys_char_group_t g;
        g.tex_key = tex_key; 
        g.first = start_index; 
        g.count = i-start_index;
        gpr_array_push_back(fsys_char_group_t, &char_groups, g);
      }
      if(i==poses->size) break;
      tex_key = gpr_array_item(poses, i).tex_key;
      start_index = i;
    }
  }

  verticies = (GLfloat*)gpr_allocate(_fontsystem.a, sizeof(GLfloat)*poses->size*8);
  tex_coord = (GLfloat*)gpr_allocate(_fontsystem.a, sizeof(GLfloat)*poses->size*8);
  for(i=0; i<poses->size; i++)
  {
    fsys_char_pos_t *pos = &gpr_array_item(poses, i);
    gpe_font_char *chr = gpr_hash_get(gpe_font_char, &font->chars, (U64)pos->c);
    U32 tmp = i*8;
    F32 line_pad_x = gpr_array_item(lines, pos->line_index);
    F32 line_pad_y = (F32)pos->line_index*font->line_height;

    tex_coord[tmp] =   chr->tex_coord[0];
    tex_coord[tmp+1] = chr->tex_coord[1];
    tex_coord[tmp+2] = chr->tex_coord[2];
    tex_coord[tmp+3] = chr->tex_coord[3];
    tex_coord[tmp+4] = chr->tex_coord[4];
    tex_coord[tmp+5] = chr->tex_coord[5];
    tex_coord[tmp+6] = chr->tex_coord[6];
    tex_coord[tmp+7] = chr->tex_coord[7];
    
    verticies[tmp] =   (GLfloat)chr->x_off+line_pad_x+pos->x;    
    verticies[tmp+1] = (GLfloat)chr->y_off+chr->h+line_pad_y;
    verticies[tmp+2] = (GLfloat)chr->x_off+line_pad_x+pos->x;
    verticies[tmp+3] = (GLfloat)chr->y_off+line_pad_y;
    verticies[tmp+4] = (GLfloat)chr->x_off+chr->w+line_pad_x+pos->x;
    verticies[tmp+5] = (GLfloat)chr->y_off+line_pad_y;
    verticies[tmp+6] = (GLfloat)chr->x_off+chr->w+line_pad_x+pos->x;
    verticies[tmp+7] = (GLfloat)chr->y_off+chr->h+line_pad_y;
  }
  
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glNewList(text->cmd_id, GL_COMPILE);
  { 
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coord);
    glVertexPointer(2, GL_FLOAT, 0, verticies);
    for(i=0; i<char_groups.size; i++)
    {
      fsys_char_group_t *group = &gpr_array_item(&char_groups, i);
      GLuint tex_id = *gpr_hash_get(GLuint, &font->tex, group->tex_key);
      glBindTexture(GL_TEXTURE_2D, tex_id);  
      glDrawArrays(GL_QUADS, group->first*4, group->count*4);
    }
    
  }glEndList();

  gpr_array_destroy(&char_groups);
  gpr_deallocate(_fontsystem.a, verticies);
  gpr_deallocate(_fontsystem.a, tex_coord);
}

void font_system_text_print (U64 id, F32 x, F32 y, gpe_text_dock dock, F32 screen_height, F32 screen_width)
{
  gpe_text *text = gpr_idlut_lookup(gpe_text, &_fontsystem.texts, id);
  switch(dock)
  {
    case DOCK_TEXT_TOP_LEFT: break;
    case DOCK_TEXT_TOP_RIGHT:
      x = screen_width - text->width - x;
      y = -y;
    break;
    case DOCK_TEXT_BOTTOM_LEFT:
      y = screen_height - text->height - y;
    break;
    case DOCK_TEXT_BOTTOM_RIGHT:
      x = screen_width - text->width - x;
      y = screen_height - text->height - y;
    break;
    case DOCK_TEXT_CENTER:
      x = (screen_width - text->width)/2 - x;
      y = (screen_height - text->height)/2 - y;
    break;
  }

  glColor3f(1.f, 1.f, 1.f);
  glPushMatrix();
  {
    glTranslatef(x, y, 0);
    glCallList(text->cmd_id);
  } glPopMatrix();
}

void font_system_text_destroy (U64 id)
{
  gpe_text *text = gpr_idlut_lookup(gpe_text, &_fontsystem.texts, id);
  glDeleteLists(text->cmd_id, 1);
  
  gpr_idlut_remove(gpe_text, &_fontsystem.texts, id);
}

void font_system_free ()
{
  U32 i, j;

  //clean texts
  for (i=0; i<_fontsystem.texts.num_items; i++)
    glDeleteLists((gpr_idlut_begin(gpe_text, &_fontsystem.texts) + i)->cmd_id, 1);
  
  gpr_idlut_destroy(gpe_font_string, &_fontsystem.texts);

  //clean fonts
  gpe_bmfont *f = gpr_hash_begin(gpe_bmfont, &_fontsystem.fonts);
  gpe_bmfont *fend = gpr_hash_end(gpe_bmfont, &_fontsystem.fonts);
  for (;f < fend; f++)
  {
    //clean chars
    gpr_hash_destroy(gpe_font_char, &f->chars);
    //clean tex
    GLuint *t = gpr_hash_begin(GLuint, &f->tex);
    GLuint *tend = gpr_hash_end(GLuint, &f->tex);
    for (;t < tend; t++) glDeleteTextures(1, t);

    gpr_hash_destroy(gpe_font_char, &f->tex);
  }
  gpr_hash_destroy(gpe_bmfont, &_fontsystem.fonts);
}

/*
 *  PRIVATE
 */

I32 _fsys_parse_common (gpe_bmfont *f, char *line)
{
  f->line_height =  _fsys_get_key_value_int(line, "lineHeight=");
  f->width =        _fsys_get_key_value_int(line, "scaleW=");
  f->height =       _fsys_get_key_value_int(line, "scaleH=");
  return 1;
}

I32 _fsys_parse_page (gpe_bmfont *f, char *line, char *path)
{
  U32 id;
  char *filename;
  char *imagepath;
  char *r;
  GLuint tex_id;
  
  r = _fsys_get_key_value(line, "id=");
  id = atoi(r);
  filename = _fsys_get_path_value (line, "file=", _fontsystem.a);
  imagepath = _strappend(path, filename, _fontsystem.a);

  
  tex_id = SOIL_load_OGL_texture (
    imagepath, 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS );

  if( tex_id == 0 )
  {
    printf("Couldn't open file \"%s\" for reading.\n", imagepath);
    gpr_deallocate(_fontsystem.a, filename);
    gpr_deallocate(_fontsystem.a, imagepath);
    return -1;
  }

  gpr_hash_set(GLuint, &f->tex, gpr_murmur_hash_64(r, strlen(r)*sizeof(char), FSYS_H_TEX_SEED), &tex_id);
    
  gpr_deallocate(_fontsystem.a, r);
  gpr_deallocate(_fontsystem.a, filename);
  gpr_deallocate(_fontsystem.a, imagepath);
  
  return 1;
}

I32 _fsys_parse_char (gpe_bmfont *f, char *line)
{
  gpe_font_char chr;

  chr.w = _fsys_get_key_value_int(line, "width=");
  chr.h = _fsys_get_key_value_int(line, "height=");
  chr.x = _fsys_get_key_value_int(line, "x=");
  chr.y = _fsys_get_key_value_int(line, "y=");
  chr.x_off = _fsys_get_key_value_int(line, "xoffset=");
  chr.y_off = _fsys_get_key_value_int(line, "yoffset=");
  chr.xAdvance = _fsys_get_key_value_int(line, "xadvance=");
  
  chr.tex_coord[2] = (GLfloat)chr.x/f->width;  
  chr.tex_coord[3] = (GLfloat)chr.y/f->height;
  chr.tex_coord[0] = (GLfloat)chr.x/f->width;
  chr.tex_coord[1] = (GLfloat)(chr.y + chr.h)/f->height;
  chr.tex_coord[6] = (GLfloat)(chr.x + chr.w)/f->width;
  chr.tex_coord[7] = (GLfloat)(chr.y + chr.h)/f->height;
  chr.tex_coord[4] = (GLfloat)(chr.x + chr.w)/f->width;
  chr.tex_coord[5] = (GLfloat)chr.y/f->height;

  {
    char *tex_id;
    tex_id = _fsys_get_key_value(line, "page=");
    chr.tex_key = gpr_murmur_hash_64(tex_id, strlen(tex_id)*sizeof(char), FSYS_H_TEX_SEED);
    gpr_deallocate(_fontsystem.a, tex_id);
  }
  {
    U64 id = _fsys_get_key_value_int(line, "id=");
    gpr_hash_set(gpe_font_char, &f->chars, id, &chr);
  }
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

  path = _fsys_get_font_path(file_path, _fontsystem.a);
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
  gpr_deallocate(_fontsystem.a, path);
  
  return state;
}

char *_fsys_get_key_value (char *line, char *key)
{
  char *result;
  char *tmp;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = _indexof(line, key);
  value_start_index = key_start_index + strlen(key);

  tmp = _substring(value_start_index, strlen(line), line, _fontsystem.a);
  
  value_end_index = _indexof(tmp, " ");
  if (value_end_index < 0) value_end_index = _indexof(tmp, "\0");

  result = _substring(0, value_end_index, tmp, _fontsystem.a);
  
  gpr_deallocate(_fontsystem.a, tmp);

  return result;
}

I32 _fsys_get_key_value_int (char *line, char *key)
{
  I32 value;
  char *r;
  r = _fsys_get_key_value(line, key);
  value = atoi(r);
  gpr_deallocate(_fontsystem.a, r);
  return value;
}

char *_fsys_get_font_path (char *filepath, gpr_allocator_t *a)
{
  char *path;
  int index;
  index = _last_indexof(filepath, '\\');

  if(index != NULL && index > 0)
    path = _substring(0, index+1, filepath, a);
  else
  {
    path = (char *) gpr_allocate(a, strlen(filepath));
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

static char *_substring (int start, int stop, char *src, gpr_allocator_t *a)
{
  char *out = (char*)gpr_allocate(a, stop-start+1);
  int count = stop - start;
  sprintf(out, "%.*s", count, src + start);
  return out;
}

I32 _indexof (char *str1, char *str2)
{
  char *tmp = strstr(str1, str2);
  return tmp - str1;
}

char *_fsys_get_path_value (char *line, char *key, gpr_allocator_t *a)
{
  char *tmp;
  char *result;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = _indexof(line, key);
  value_start_index = key_start_index + strlen(key) + 1;

  tmp = _substring(value_start_index, strlen(line), line, a);
  
  value_end_index = _indexof(tmp, "\"");

  result = _substring(0, value_end_index, tmp, a);
  
  gpr_deallocate(a, tmp);
  return result;
}

static char *_strappend (char *str1, char *str2, gpr_allocator_t *a)
{
  char *s = (char *)gpr_allocate(a, (strlen(str1) + strlen(str2) +1)*sizeof(char));
  sprintf(s, "%s%s", str1, str2);
  return s;
}