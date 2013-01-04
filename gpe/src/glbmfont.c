#include "glbmfont.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SOIL\SOIL.h>

const int TOP_LEFT      = 0;
const int BOTTOM_LEFT   = 1;
const int BOTTOM_RIGHT  = 2;
const int TOP_RIGHT     = 3;
const int CHAR_MISS     = 248; //char chelou

int   _glbmfont_getKeyValue   (char *line, char *key);
int   _glbmfont_indexof       (char *str1, char *str2);
int   _glbmfont_last_indexof  (char *str1, char *str2);
int   _glbmfont_getLineWidth  (glbmfont *font, char *text);
void  _glbmfont_substring     (int start, int stop, char *text, char *out);
char *_glbmfont_getPathValue  (char * line, char *key);
char *_glbmfont_getFilepath   (char *filepath);
char *_glbmfont_strAppend     (char *str1, char *str2);
int   _glbmfont_count_lines   (glbmfont *font, char *text);
int   _glbmfont_parse_common  (glbmfont *font, char *line);
int   _glbmfont_parse_page    (glbmfont *font, char *line, char *path);
int   _glbmfont_parse_char    (glbmfont *font, char *line);
void  _glbmfont_renderLine    (glbmfont *font, char *line, FVect padding, float screen_width,
                                float screen_height, float scale, gpeDock dock);
FVect _glbmfont_getLine_padd  (glbmfont *font, gpeDock dock, 
                                char *line, int line_index, float line_height, int line_count, 
                                float pad_x , float pad_y, 
                                float screen_width, float screen_height, float scale);
float _glbmfont_renderChar    (glbmfont_char *chrDesc, FVect padding, 
                                float screen_width, float screen_height, float scale);

glbmfont _glbmfont;

/*
 *  PRIVATE
 */

int   _glbmfont_indexof(char *str1, char *str2)
{
  char *tmp = strstr(str1, str2);
  return tmp - str1;
}

int   _glbmfont_last_indexof(char *str, char chr)
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

void  _glbmfont_substring(int start, int stop, char *src, char *out)
{
   int count = stop - start;
   sprintf(out, "%.*s", count, src + start);
}

int   _glbmfont_getKeyValue(char *line, char *key)
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

char *_glbmfont_getPathValue(char *line, char *key)
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

char *_glbmfont_strAppend(char *str1, char *str2)
{
  char *s = (char *)malloc((strlen(str1) + strlen(str2) +1)*sizeof(char));
  sprintf(s, "%s%s", str1, str2);
  return s;
}

char *_glbmfont_getFilepath(char *filepath)
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

int   _glbmfont_getLineWidth(glbmfont *font, char *text)
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
      charWidth = font->chars[CHAR_MISS].xAdvance;
    }
    else
    {
      charWidth = font->chars[chr].xAdvance;
    }
    value = value + charWidth;
  }

  return value;
}

int   _glbmfont_count_lines (glbmfont *font, char *text)
{
  int count, i, len;
  count=1;
  len = strlen(text);
  
  for (i=0; i < len; i++)
  {
    if(text[i] == '\n')
    {
     count = count +1;
    }
  }
  
  return count;
}

/*
 *  LOAD: private parsing stuff
*/

int   _glbmfont_parse_common(glbmfont *font, char *line)
{
  font->lineHeight = _glbmfont_getKeyValue(line, "lineHeight=");
  font->width =      _glbmfont_getKeyValue(line, "scaleW=");
  font->height =     _glbmfont_getKeyValue(line, "scaleH=");
  font->pages =      _glbmfont_getKeyValue(line, "pages=");

  return 1;
}

int   _glbmfont_parse_page(glbmfont *font, char *line, char *path)
{
  int id;
  char *filename;
  char *imagepath;

  id =        _glbmfont_getKeyValue(line, "id=");
  filename =  _glbmfont_getPathValue(line, "file=");
  imagepath = _glbmfont_strAppend(path, filename);
    
  //printf("Loading image file:\n\t\"%s\"\n", imagepath);
  font->tex_2d[id] = SOIL_load_OGL_texture (
    imagepath, 
    SOIL_LOAD_AUTO, 
    SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO );

  if( font->tex_2d[id] == 0 )
  {
    printf("Couldn't open file \"%s\" for reading.\n", imagepath);
    free(filename);
    free(imagepath);
    return -1;
  } 
    
  free(filename);
  free(imagepath);

  font->tex_count = font->tex_count + 1;

  return 1;
}

int   _glbmfont_parse_char(glbmfont *font, char *line)
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

  //calc des 4 points de la texture du char
  // sens trigo : origine bord haut gauche
  // conversion des coord des points de pixel vers coord dans gl (float)
  charDesc.texCoord[TOP_LEFT].x =     (GLfloat)((float)charDesc.x / (float)font->width);
  charDesc.texCoord[TOP_LEFT].y =     (GLfloat)((float)charDesc.y / (float)font->height);
  charDesc.texCoord[BOTTOM_LEFT].x =  (GLfloat)((float)charDesc.x / (float)font->width);
  charDesc.texCoord[BOTTOM_LEFT].y =  (GLfloat)((float)(charDesc.y + charDesc.height) / (float)font->height);                     
  charDesc.texCoord[BOTTOM_RIGHT].x = (GLfloat)((float)(charDesc.x + charDesc.width) / (float)font->width);
  charDesc.texCoord[BOTTOM_RIGHT].y = (GLfloat)((float)(charDesc.y + charDesc.height) / (float)font->height);
  charDesc.texCoord[TOP_RIGHT].x =    (GLfloat)((float)(charDesc.x + charDesc.width) / (float)font->width);
  charDesc.texCoord[TOP_RIGHT].y =    (GLfloat)((float)charDesc.y / (float)font->height);
  
  font->chars[id] = charDesc;

  return 1;
}

/*
 *  PRINT: private rendering stuff
 */

void  _glbmfont_renderLine (glbmfont *font, char *line, FVect padding,
                            float screen_width, float screen_height, 
                            float scale, gpeDock dock)
{
  int i, len;
  char chr;
  glbmfont_char *chrDesc;
  
  len = strlen(line);
  
  for (i=0; i<len; i++)
  {
    glColor3f(1.f, 1.f, 1.f);
    chr = line[i];
    if (chr < 0 || chr > GLBMFONT_CHARS_SIZE)
    {
      //le code ascii de la lettre dépasse l'index du tableau
      chrDesc = &font->chars[CHAR_MISS];
      glColor3f(1.f, .0f, .0f);
    }
    else
    {
      chrDesc = &font->chars[chr];
      if(chrDesc->width <= 0)
      { 
        //lettre courrante n'est pas dans le sprite
        chrDesc = &font->chars[CHAR_MISS];
        glColor3f(0.f, .0f, 1.f);
      }
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font->tex_2d[chrDesc->page]);
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    
    padding.x = _glbmfont_renderChar(chrDesc, padding, screen_width, screen_height, scale);
    
    glDisable(GL_TEXTURE_2D);
  }
}

float _glbmfont_renderChar (glbmfont_char *chrDesc, FVect padding, 
                            float screen_width, float screen_height, float scale)
{
  DVect quads[4];
  quads[BOTTOM_LEFT].x =  (GLdouble) (padding.x + ((float)chrDesc->xOffset / screen_width) * scale);
  quads[BOTTOM_LEFT].y =  (GLdouble)  padding.y - ((float)(chrDesc->yOffset + chrDesc->height)/screen_height)* scale;
  quads[TOP_LEFT].x =     (GLdouble) (padding.x + ((float)chrDesc->xOffset / screen_width) * scale);
  quads[TOP_LEFT].y =     (GLdouble) (padding.y - ((float)chrDesc->yOffset/screen_height) * scale);
  quads[TOP_RIGHT].x =    (GLdouble) (padding.x + ((float)(chrDesc->xOffset + chrDesc->width)/screen_width) * scale);
  quads[TOP_RIGHT].y =    (GLdouble) (padding.y - ((float)chrDesc->yOffset/screen_height) * scale);
  quads[BOTTOM_RIGHT].x = (GLdouble) (padding.x + ((float)(chrDesc->xOffset + chrDesc->width)/screen_width) * scale);
  quads[BOTTOM_RIGHT].y = (GLdouble)  padding.y - ((float)(chrDesc->yOffset + chrDesc->height)/screen_height)* scale;

  glBegin(GL_QUADS);
  {             
 	  glTexCoord2f( chrDesc->texCoord[TOP_LEFT].x,
                  chrDesc->texCoord[TOP_LEFT].y );
 	  glVertex2d(	  quads[TOP_LEFT].x, 
                  quads[TOP_LEFT].y );
     
 	  glTexCoord2f(	chrDesc->texCoord[BOTTOM_LEFT].x, 
                  chrDesc->texCoord[BOTTOM_LEFT].y );
 	  glVertex2d(	  quads[BOTTOM_LEFT].x, 
                  quads[BOTTOM_LEFT].y );
 
 	  glTexCoord2f(	chrDesc->texCoord[BOTTOM_RIGHT].x, 
                  chrDesc->texCoord[BOTTOM_RIGHT].y) ;
 	  glVertex2d(	  quads[BOTTOM_RIGHT].x, 
                  quads[BOTTOM_RIGHT].y );
 
 	  glTexCoord2f(	chrDesc->texCoord[TOP_RIGHT].x, 
                  chrDesc->texCoord[TOP_RIGHT].y) ;
 	  glVertex2d(	  quads[TOP_RIGHT].x, 
                  quads[TOP_RIGHT].y );
	} glEnd();

  return padding.x + (float)chrDesc->xAdvance/screen_width * scale;
}


FVect _glbmfont_getLine_padd (glbmfont *font, gpeDock dock, 
                              char *line, int line_index, float line_height, int line_count, 
                              float pad_x , float pad_y, 
                              float screen_width, float screen_height, float scale)
{
  float line_width;
  FVect padding;
  
  switch (dock)
  {
    case dock_top_left:
      padding.x = pad_x - 1.f;
      padding.y = 1.f - (line_index * line_height);
      break;
    case dock_top_right:
      line_width = (float)_glbmfont_getLineWidth(font, line);
      padding.x = 1.f - pad_x - (line_width/screen_width) * scale;
      padding.y = 1.f - (line_index * line_height);
      break;
    case dock_center:
      line_width = (float)_glbmfont_getLineWidth(font, line) * scale;
      padding.x = pad_x - (line_width/2.f)/screen_width;
      padding.y = pad_y + line_height * (((float)line_count / 2) - line_index );
      break;
    case dock_bottom_left:
      padding.x = pad_x - 1.f;
      padding.y = pad_y + ((line_index+1) * line_height) - 1.f;
      break;
    case dock_bottom_right:
      line_width = (float)_glbmfont_getLineWidth(font, line);
      padding.x = 1.f - pad_x - (line_width/screen_width) * scale;
      padding.y = pad_y + ((line_index+1) * line_height) - 1.f;
      break;
  }
  return padding;
}


/*
 *  PUBLIC
 */

int   glbmfont_load(void)
{
  return glbmfont_load("D:\\temp\\QuicksandBook.fnt");
}

int   glbmfont_load(char *filepath)
{ 
  int state = 1;
  char line_buffer[BUFSIZ];
  char *path;
  FILE *file = fopen(filepath, "r");
  glbmfont font;

  if (!file) {
      printf("Couldn't open file \"%s\" for reading.\n", filepath);
      return -1;
  }

  font.tex_count = 0;

  path = _glbmfont_getFilepath(filepath);
  while (fgets(line_buffer, sizeof(line_buffer), file) && state == 1)
  {
    if (_glbmfont_indexof (line_buffer, "common ") > -1)
    {
      state = _glbmfont_parse_common(&font, line_buffer);
    }
    else if (_glbmfont_indexof(line_buffer, "page ") > -1)
    {    
      state = _glbmfont_parse_page(&font, line_buffer, path);
    }
    else if (_glbmfont_indexof(line_buffer, "char ") > -1)
    {
      state = _glbmfont_parse_char(&font, line_buffer);
    }
  }

  fclose(file);
  free(path);
  
  _glbmfont = font;

  return state;
}

void  glbmfont_print(char *text, int x, int y, gpeDock dock)
{
  const float scale = 2.f;
  glbmfont_print(text, x, y, dock, scale);
}

void  glbmfont_print(char *text, int x, int y, gpeDock dock, float scale)
{
  int i, begin_index, len, line_index, line_count;
  int screen_width, screen_height;
  float f_screen_width, f_screen_height;
  float pad_x, pad_y, line_height;
  char *buffer;
  FVect padding;

  len = strlen(text);
  if(len == 0)
  {
    return;
  }

	glfwGetWindowSize(&screen_width, &screen_height);
  
  f_screen_width = (float)screen_width;
  f_screen_height = (float)screen_height;
  pad_x = (float)x/f_screen_width;
  pad_y = (float)y/f_screen_height;
  line_index = 0;
  begin_index = 0;
  line_count = _glbmfont_count_lines(&_glbmfont, text);
  line_height = (float)_glbmfont.lineHeight / screen_height * scale;
  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
  for (i=0; i<len; i++)
  {
    if(text[i] == '\n')
    {
      buffer = (char *) malloc((i - begin_index + 1)*sizeof(char));
      _glbmfont_substring(begin_index, i, text, buffer);
      
      padding = _glbmfont_getLine_padd(&_glbmfont, dock, buffer, line_index, line_height, line_count, pad_x, pad_y, f_screen_width, f_screen_height, scale);
      _glbmfont_renderLine(&_glbmfont, buffer, padding, f_screen_width, f_screen_height, scale, dock);

      free(buffer);
      begin_index = i+1;
      line_index = line_index+1;
    }
  }

  //draw last line
  if(begin_index < len)
  {
    buffer = (char *) malloc((len - begin_index+1)*sizeof(char));
    _glbmfont_substring(begin_index, len, text, buffer);
    
    padding = _glbmfont_getLine_padd(&_glbmfont, dock, buffer, line_index, line_height, line_count, pad_x, pad_y, f_screen_width, f_screen_height, scale);
    _glbmfont_renderLine(&_glbmfont, buffer, padding, f_screen_width, f_screen_height, scale, dock);
    
    free(buffer);
  }

  glDisable(GL_BLEND);
  glBlendFunc(GL_NONE, GL_NONE);
}

void  glbmfont_free(void)
{
  int i, tex_count;
  tex_count = _glbmfont.tex_count;

  if(tex_count<1)
  {
    return;
  }

  for( i=0; i< tex_count; i++)
  {
    glDeleteTextures(1, &_glbmfont.tex_2d[i]);
  }
  _glbmfont.tex_count = 0;
}