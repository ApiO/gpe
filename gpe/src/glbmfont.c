#include "glbmfont.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SOIL\SOIL.h>

#define CHAR_TEX_BORDER_TOP_LEFT      0
#define CHAR_TEX_BORDER_BOTTOM_LEFT   1
#define CHAR_TEX_BORDER_BOTTOM_RIGHT  2
#define CHAR_TEX_BORDER_TOP_RIGHT     3
#define CHAR_MISSING_REPLACE          88 //X char

int   _glbmfont_parse_common (char *line);
int   _glbmfont_parse_page (char *line, char *path);
int   _glbmfont_parse_char (char *line, int font_width, int font_height);
int   _glbmfont_getKeyValue (char * line, char *key);
int   _glbmfont_indexof (char *str1, char *str2);
int   _glbmfont_last_indexof (char *str1, char *str2);
int   _glbmfont_getLineWidth (glbmfont *font, char *text);
void   _glbmfont_renderLine (glbmfont_char *chrDesc, int x, int y, 
                            int screen_width, int screen_height, float scale);
int   _glbmfont_renderChar( glbmfont_char *chrDesc, float x, float y, 
                            int screen_width, int screen_height, 
                            int padX, int padY,
                            float scale);
void  _glbmfont_substring (int start, int stop, char *text, char *out);
char *_glbmfont_getPathValue (char * line, char *key);
char *_glbmfont_getFilepath (char *filepath);
char *_glbmfont_strAppend (char *str1, char *str2);

glbmfont _glbmfont;

int 
_glbmfont_indexof(char *str1, char *str2)
{
  char *tmp = strstr(str1, str2);
  return tmp - str1;
}

int 
_glbmfont_last_indexof(char *str, char chr)
{
  int i, len;
  len = strlen(str);
  for (i=len-1; i>=0; i--)
  {
    if(str[i] == chr)
    {
      return i;
    }
  }
  return NULL;
}

void 
_glbmfont_substring(int start, int stop, char *src, char *out)
{
   int count = stop - start;
   sprintf(out, "%.*s", count, src + start);
}

int 
_glbmfont_getKeyValue(char *line, char *key)
{
  int value;
  char *result;
  char *tmp;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = _glbmfont_indexof(line, key);
  value_start_index = key_start_index + strlen(key);

  tmp = (char *) malloc((strlen(line)-value_start_index+1)*sizeof(char));
  _glbmfont_substring(value_start_index, strlen(line)-value_start_index, line, tmp);
  
  value_end_index = _glbmfont_indexof(tmp, " ");
  if (value_end_index < 0)
  {
    value_end_index = _glbmfont_indexof(tmp, "\0");
  }

  result = (char *) malloc((value_end_index+1)*sizeof(char));
  _glbmfont_substring(0, value_end_index, tmp, result);
  
  value = atoi(result);

  free(result);
  free(tmp);

  //printf("KEY:\"%s\" VALUE:\"%d\"\n", key, value);

  return value;
}

char *
_glbmfont_getPathValue(char *line, char *key)
{
  char *tmp;
  char *result;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = _glbmfont_indexof(line, key);
  value_start_index = key_start_index + strlen(key) + 1;

  tmp = (char *) malloc((strlen(line)-value_start_index+1)*sizeof(char));
  _glbmfont_substring(value_start_index, strlen(line), line, tmp);
  
  value_end_index = _glbmfont_indexof(tmp, "\"");

  result = (char *) malloc((value_end_index+1)*sizeof(char));
  _glbmfont_substring(0, value_end_index, tmp, result);
  
  free(tmp);
  return result;
}

char *
_glbmfont_strAppend(char *str1, char *str2)
{
  char *s = (char *)malloc((strlen(str1) + strlen(str2) +1)*sizeof(char));
  sprintf(s, "%s%s", str1, str2);
  return s;
}

char *
_glbmfont_getFilepath(char *filepath)
{
  char *path;
  int index;
  index = _glbmfont_last_indexof(filepath,'\\');

  if(index != NULL && index > 0)
  { 
    path = (char *) malloc(index+2);
    _glbmfont_substring(0, index+1, filepath, path);
  }
  else
  {
    path = (char *) malloc(strlen(filepath));
    strcpy(path, path);
  }
  return path;
}

int 
_glbmfont_parse_common(char *line)
{
  _glbmfont.lineHeight = _glbmfont_getKeyValue(line, "lineHeight=");
  _glbmfont.base =       _glbmfont_getKeyValue(line, "base=");
  _glbmfont.width =      _glbmfont_getKeyValue(line, "scaleW=");
  _glbmfont.height =     _glbmfont_getKeyValue(line, "scaleH=");
  _glbmfont.pages =      _glbmfont_getKeyValue(line, "pages=");

  return 1;
}

int 
_glbmfont_parse_page(char *line, char *path)
{
  int id;
  char *filename;
  char *imagepath;

  id =        _glbmfont_getKeyValue(line, "id=");
  filename =  _glbmfont_getPathValue(line, "file=");
  imagepath = _glbmfont_strAppend(path, filename);
    
  //printf("Loading image file:\n\t\"%s\"\n", imagepath);
  _glbmfont.tex_2d[id] = SOIL_load_OGL_texture (
    imagepath, 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  if( _glbmfont.tex_2d[id] == 0 )
  {
    printf("Couldn't open file \"%s\" for reading.\n", imagepath);
    free(filename);
    free(imagepath);
    return -1;
  } 
    
  free(filename);
  free(imagepath);

  return 1;
}

int 
_glbmfont_parse_char(char *line, int font_width, int font_height)
{
  glbmfont_char charDesc;
  int id;

  id =                _glbmfont_getKeyValue(line, "id=");
  charDesc.x =        _glbmfont_getKeyValue(line, "x=");
  charDesc.y =        _glbmfont_getKeyValue(line, "y=");
  charDesc.width =    _glbmfont_getKeyValue(line, "width=");
  charDesc.height =   _glbmfont_getKeyValue(line, "height=");
  charDesc.xOffset =  _glbmfont_getKeyValue(line, "xoffset=");
  charDesc.yOffset =  _glbmfont_getKeyValue(line, "yoffset=");
  charDesc.xAdvance = _glbmfont_getKeyValue(line, "xadvance=");
  charDesc.page =     _glbmfont_getKeyValue(line, "page=");
  charDesc.chnl =     _glbmfont_getKeyValue(line, "chnl=");

  //calc des 4 points de la texture du char
  // sens trigo : origine bord haut gauche
  // conversion des coord des points de pixel vers coord dans gl (float)
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_LEFT].x =     (GLfloat)((float)charDesc.x / (float)font_width);
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_LEFT].y =     (GLfloat)((float)charDesc.y / (float)font_height);
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].x =  (GLfloat)((float)charDesc.x / (float)font_width);
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].y =  (GLfloat)((float)(charDesc.y + charDesc.height) / (float)font_height);                     
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].x = (GLfloat)((float)(charDesc.x + charDesc.width) / (float)font_width);
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].y = (GLfloat)((float)(charDesc.y + charDesc.height) / (float)font_height);
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_RIGHT].x =    (GLfloat)((float)(charDesc.x + charDesc.width) / (float)font_width);
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_RIGHT].y =    (GLfloat)((float)charDesc.y / (float)font_height);
  
  _glbmfont.chars[id] = charDesc;

  return 1;
}

int 
_glbmfont_getLineWidth(glbmfont *font, char *text)
{
  int i, len, charWidth, value;
  char chr;
  value = 0;
  len = strlen(text);

  for(i=0; i<len ; i++)
  {
    chr = text[i];
    if (chr < 0 || chr > GLBMFONT_CHARS_SIZE)
    {
      charWidth = _glbmfont.chars[CHAR_MISSING_REPLACE].xAdvance;
    }
    else
    {
      charWidth = font->chars[chr].xAdvance;
    }
    value = value + charWidth;
  }

  return value;
}

void 
_glbmfont_renderLine (glbmfont_char *chrDesc, int x, int y, int screen_width, int screen_height, float scale)
{
}

int 
_glbmfont_renderChar (glbmfont_char *chrDesc, float posX, float posY, int screen_width, int screen_height, int padX, int padY, float scale)
{
  glBegin(GL_QUADS);
  {             
 	  glTexCoord2f( chrDesc->texCoord[CHAR_TEX_BORDER_TOP_LEFT].x,
                  chrDesc->texCoord[CHAR_TEX_BORDER_TOP_LEFT].y );
 	  glVertex2d(	(GLdouble) (posX + ((float)(chrDesc->xOffset + padX)/(float)screen_width) * scale), 
                (GLdouble) (posY + ((float)(chrDesc->height)/(float)screen_height) * scale) );
     
 	  glTexCoord2f(	chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].x, 
                  chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].y );
 	  glVertex2d(	(GLdouble)(posX + ((float)(chrDesc->xOffset + padX)/(float)screen_width) * scale), 
                (GLdouble) posY);
 
 	  glTexCoord2f(	chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].x, 
                  chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].y) ;
 	  glVertex2d(	(GLdouble) (posX + ((float)(chrDesc->xOffset + padX + chrDesc->width)/(float)screen_width) *scale), 
                (GLdouble) posY);
 
 	  glTexCoord2f(	chrDesc->texCoord[CHAR_TEX_BORDER_TOP_RIGHT].x, 
                  chrDesc->texCoord[CHAR_TEX_BORDER_TOP_RIGHT].y) ;
 	  glVertex2d(	(GLdouble) (posX + ((float)(chrDesc->xOffset + padX + chrDesc->width)/(float)screen_width) * scale), 
                (GLdouble) (posY + ((float)(chrDesc->height)/(float)screen_height) * scale) );

	} glEnd();
  return padX + chrDesc->xAdvance;
}

int 
glbmfont_load(void)
{
  return glbmfont_load("C:\\temp\\QuicksandBook.fnt");
}

int 
glbmfont_load(char *filepath)
{ 
  int state = 1;
  char line_buffer[BUFSIZ];
  char *path;
  FILE *file = fopen(filepath, "r");

  if (!file) {
      printf("Couldn't open file \"%s\" for reading.\n", filepath);
      return -1;
  }

  //printf("Loading font:\n\t\"%s\"\n", filepath);
  path = _glbmfont_getFilepath(filepath);
  while (fgets(line_buffer, sizeof(line_buffer), file) && state == 1)
  {
    if (_glbmfont_indexof (line_buffer, "common ") > -1)
    {
      state = _glbmfont_parse_common(line_buffer);
    }
    else if (_glbmfont_indexof(line_buffer, "page ") > -1)
    {    
      state = _glbmfont_parse_page(line_buffer, path);
    }
    else if (_glbmfont_indexof(line_buffer, "char ") > -1)
    {
      state = _glbmfont_parse_char(line_buffer, _glbmfont.width, _glbmfont.height);
    }
  }

  fclose(file);
  free(path);

  return state;
}

void 
glbmfont_print(char *text, int x, int y, gpeDock dock)
{
  int i, len, padX, padY, line_width, line_height;
  int screen_width, screen_height;
  char chr;
  float posX, posY;
  glbmfont_char chrDesc;

  padX = 0;
  padY = 0;

	glfwGetWindowSize(&screen_width, &screen_height);
    
  len = strlen(text);

  //DEV
  glColor3f(.42f, .42f, .8f);
  glBegin(GL_LINES);
    glVertex2f(0.f, 1.f);
    glVertex2f(0.f, -1.f);
  glEnd( );
  glBegin(GL_LINES);
    glVertex2f(-1.f, 0.f);
    glVertex2f(1.f, 0.f);
  glEnd( );
  //DEV

  switch (dock)
  {
    case dock_top_left:
      posX = ((float)x/(float)screen_width) - 1.f;
      posY = 1.f - ((float)(_glbmfont.lineHeight + y)/(float)screen_height);
      break;
    case dock_top_right:
      line_width = _glbmfont_getLineWidth(&_glbmfont, text);
      posX = 1.f - ((float)(x + line_width*2)/(float)screen_width);
      posY = 1.f - ((float)(_glbmfont.lineHeight + y)/(float)screen_height);
      break;
    case dock_center:
      line_width = _glbmfont_getLineWidth(&_glbmfont, text);
      posX = (float)(x - line_width)/(float)screen_width;
      posY = (float)y/(float)screen_height;
      break;
    case dock_bottom_left:
      posX = ((float)x/(float)screen_width) - 1.f;
      posY = ((float)y/(float)screen_height) - 1.f;
      break;
    case dock_bottom_right:
      line_width = _glbmfont_getLineWidth(&_glbmfont, text);
      posX = 1.f - ((float)(x + line_width*2)/(float)screen_width);
      posY = ((float)y/(float)screen_height) - 1.f;
      break;
  }
  
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
  for (i=0; i<len; i++)
  {
    if(text[i] == '\n')
    {
      padY = padY + _glbmfont.lineHeight;
    }
    else
    {
      glColor3f(1.f, 1.f, 1.f);
      chr = text[i];
      if (chr < 0 || chr > GLBMFONT_CHARS_SIZE)
      {
        //le code ascii de la lettre dépasse l'index du tableau
        chrDesc = _glbmfont.chars[CHAR_MISSING_REPLACE];
        glColor3f(1.f, .0f, .0f);
      }
      else
      {
        chrDesc = _glbmfont.chars[chr];
        if(chrDesc.width == 0)
        { 
          //lettre courrante n'est pas dans le sprite
          chrDesc = _glbmfont.chars[CHAR_MISSING_REPLACE];
          glColor3f(0.f, .0f, 1.f);
        }
      }

      glBindTexture(GL_TEXTURE_2D, _glbmfont.tex_2d[chrDesc.page]);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

      padX = _glbmfont_renderChar(&chrDesc, posX, posY, screen_width, screen_height, padX, padY, 2.f);
    }
  }
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

void 
glbmfont_free(void)
{
  //SOIL_free_image_data(BMFont.tex_2d);
}