#include "glbmfont.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SOIL\SOIL.h>

#define CHAR_TEX_BORDER_TOP_LEFT      0
#define CHAR_TEX_BORDER_BOTTOM_LEFT   1
#define CHAR_TEX_BORDER_BOTTOM_RIGHT  2
#define CHAR_TEX_BORDER_TOP_RIGHT     3

int   _glbmfont_parse_common (char *line);
int   _glbmfont_parse_page (char *line, char *path);
int   _glbmfont_parse_char (char *line);
int   _glbmfont_getKeyValue (char * line, char *key);
int   _glbmfont_indexof (char *str1, char *str2);
int   _glbmfont_last_indexof (char *str1, char *str2);
void  _glbmfont_renderChar (glbmfont_char *chrDesc, int *x, int *y, int *padX, int *screen_width, int *screen_height);
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
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_POWER_OF_TWO );

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
_glbmfont_parse_char(char *line)
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
  // conversion des coord en pixel des points en coord dans gl (float)
  // au chargement de la texture Y est inversé pour que le fnt soit lisible naturellement
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_LEFT].x =     (GLfloat)(charDesc.x / charDesc.width);
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_LEFT].y =     (GLfloat)(charDesc.y / charDesc.height);
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].x =  (GLfloat)(charDesc.x / charDesc.width);
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].y =  (GLfloat)((charDesc.y + charDesc.height) / charDesc.height);                     
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].x = (GLfloat)((charDesc.x + charDesc.width) / charDesc.width);
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].y = (GLfloat)((charDesc.y + charDesc.height) / charDesc.height);
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_RIGHT].x =    (GLfloat)((charDesc.x + charDesc.width) / charDesc.width);
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_RIGHT].y =    (GLfloat)(charDesc.y / charDesc.height);
  
  _glbmfont.chars[id] = charDesc;

  return 1;
}

void 
_glbmfont_renderChar (glbmfont_char *chrDesc, int *x, int *y, int *padX, int *screen_width, int *screen_height)
{
  glBegin(GL_QUADS);
  {
	  glTexCoord2f(	chrDesc->texCoord[CHAR_TEX_BORDER_TOP_LEFT].x,
                  chrDesc->texCoord[CHAR_TEX_BORDER_TOP_LEFT].y );
	  //glVertex2d(	(GLdouble)-0.5, (GLdouble)0.5 );
	  glTexCoord2f(	chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].x, 
                  chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].y );
	  //glVertex2d(	(GLdouble)-0.5, (GLdouble)0.5 );
	  glTexCoord2f(	chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].x, 
                  chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].y );
	  //glVertex2d(	(GLdouble)-0.5, (GLdouble)0.5 );
	  glTexCoord2f(	chrDesc->texCoord[CHAR_TEX_BORDER_TOP_RIGHT].x, 
                  chrDesc->texCoord[CHAR_TEX_BORDER_TOP_RIGHT].y );
	  //glVertex2d(	(GLdouble)-0.5, (GLdouble)0.5 );
	} glEnd();
}

int 
glbmfont_load(void)
{
  return glbmfont_load("C:\\Users\\utilisateur\\Desktop\\BMFont ressources\\QuicksandBook.fnt");
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
      state = _glbmfont_parse_char(line_buffer);
    }
  }

  fclose(file);
  free(path);

  return state;
}

void 
glbmfont_print(char *text, int x, int y)
{
  int i, len, padX;
  char chr;
  int screen_width, screen_height;
  glbmfont_char chrDesc;

	glfwGetWindowSize(&screen_width, &screen_height);
    
  len = strlen(text);
  
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
  for (i=0; i<len; i++)
  {
    chr = text[i];
    if (chr > GLBMFONT_CHARS_SIZE)
    {
      //print d'un "#", le code ascii de la lettre dépasse l'index du tableau
      chrDesc = _glbmfont.chars[35];
      printf("Char:\'%c\' asc:%d > glbmfont.Chars[%d]\n", chr, chr, GLBMFONT_CHARS_SIZE);
    }
    else
    { 
      chrDesc = _glbmfont.chars[chr];
      if(chrDesc.width == 0)
      { 
        //print d'un "?", lettre courrante n'est pas dans le sprite
        chrDesc = _glbmfont.chars[63];
        printf("Char:\'%c\' asc:%d, not in sprite\n", chr, chr);
      }
    }

    //In C, you can use the char data type just like a number and it will automatically convert it to the ASCII value
    // ex: int i = '!' <=>  i = 33;
    //printf("\t[char index]=%d, [char]=\'%c\', [ascii code]=%d, [FONT x]=%d, [FONT y]=%d\n", i, text[i], text[i], chrDesc.x , chrDesc.y);
    glBindTexture(GL_TEXTURE_2D, _glbmfont.tex_2d[chrDesc.page]);
    //Filtering
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    _glbmfont_renderChar(&chrDesc, &x, &y, &padX, &screen_width, &screen_height);
    //posX = posX + chrDesc + chrDesc.xAdvance;
  }
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D); //unbind texture
}

void 
glbmfont_free(void)
{
  //SOIL_free_image_data(BMFont.tex_2d);
}