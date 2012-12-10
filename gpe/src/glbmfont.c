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
void  _glbmfont_renderChar (glbmfont_char *chrDesc, int *padX, int *padY);
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
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT );

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

  //printf("char\n");
  id = _glbmfont_getKeyValue(line, "id=");
  charDesc.x =        _glbmfont_getKeyValue(line, "x=");
  charDesc.y =        _glbmfont_getKeyValue(line, "y=");
  charDesc.width =    _glbmfont_getKeyValue(line, "width=");
  charDesc.height =   _glbmfont_getKeyValue(line, "height=");
  charDesc.xOffset =  _glbmfont_getKeyValue(line, "xoffset=");
  charDesc.yOffset =  _glbmfont_getKeyValue(line, "yoffset=");
  charDesc.xAdvance = _glbmfont_getKeyValue(line, "xadvance=");
  charDesc.page =     _glbmfont_getKeyValue(line, "page=");
  charDesc.chnl =     _glbmfont_getKeyValue(line, "chnl=");

  //les coord des 4 points de la texture du char : sens trigo, origine bord haut gauche
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_LEFT].x =     charDesc.x;
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_LEFT].y =     charDesc.y;
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].x =  charDesc.x;
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].y =  charDesc.y + charDesc.height;                     
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].x = charDesc.x + charDesc.width;
  charDesc.texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].y = charDesc.y + charDesc.height;
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_RIGHT].x =    charDesc.x + charDesc.width;
  charDesc.texCoord[CHAR_TEX_BORDER_TOP_RIGHT].y =    charDesc.y;

  _glbmfont.chars[id] = charDesc;

  return 1;
}


void 
_glbmfont_renderChar (glbmfont_char *chrDesc, int padX, int padY)
{
  int x, y;
  glBegin(GL_QUADS);
  {
    x = chrDesc->texCoord[CHAR_TEX_BORDER_TOP_LEFT].x;
    y = chrDesc->texCoord[CHAR_TEX_BORDER_TOP_LEFT].y;
	  //glTexCoord2f(	(GLfloat)x, (GLfloat)y );
	  glTexCoord2f(	(GLfloat)68, (GLfloat)20 );
	  glVertex2d(	(GLdouble)-0.5, (GLdouble)0.5 );
      
    x = chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].x;
    y = chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_LEFT].y;
	  //glTexCoord2f(	(GLfloat)x, (GLfloat)y );
	  glTexCoord2f(	(GLfloat)68, (GLfloat)53 );
	  glVertex2d(	(GLdouble)-0.5, (GLdouble) -0.5);
                   
    x = chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].x;
    y = chrDesc->texCoord[CHAR_TEX_BORDER_BOTTOM_RIGHT].y;  
	  //glTexCoord2f(	(GLfloat)x, (GLfloat)y );
	  glTexCoord2f(	(GLfloat)101, (GLfloat)53 );
	  glVertex2d(	(GLdouble)0.5, (GLdouble)-0.5 );
                
    x = chrDesc->texCoord[CHAR_TEX_BORDER_TOP_RIGHT].x;
    y = chrDesc->texCoord[CHAR_TEX_BORDER_TOP_RIGHT].y;     
	  //glTexCoord2f(	(GLfloat)x, (GLfloat)y );
	  glTexCoord2f(	(GLfloat)101, (GLfloat) 20 );
	  glVertex2d(	(GLdouble)0.5, (GLdouble)0.5 );

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
  int i, len, posX;
  char chr;
  posX = 0;
  glbmfont_char chrDesc;
  
  len = strlen(text);
  
  glEnable(GL_TEXTURE_2D);
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
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    _glbmfont_renderChar(&chrDesc, x, y);
    x = x + chrDesc.width;
  }
  glDisable(GL_TEXTURE_2D); //unbind texture too
}

void 
glbmfont_free(void)
{
  //SOIL_free_image_data(BMFont.tex_2d);
}

GLuint img;
void _DEV_init(void)
{
    img = SOIL_load_OGL_texture (
      "d:\\temp.png", 
      SOIL_LOAD_AUTO, 
      SOIL_CREATE_NEW_ID, 
		  SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MULTIPLY_ALPHA);
}

void _DEV_draw(void)
{
  glEnable(GL_TEXTURE_2D);
  {
    glBindTexture(GL_TEXTURE_2D, img);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    /*
    //affiche le centre de l'image en plein écran
    glBegin(GL_QUADS);
    {
	    glTexCoord2f(	.25f, .75f );
	    glVertex2d(	-1, 1 );
    
	    glTexCoord2f(	.25f, .25f );
	    glVertex2d(	-1, -1 );
    
	    glTexCoord2f(	.75f, .25f );
	    glVertex2d(	1, -1 );
    
	    glTexCoord2f(	.75f, .75f );
	    glVertex2d( 1, 1 );
    } glEnd();
    */
    /*
    //afficher l'image en plein écran
    glBegin(GL_QUADS);
    {
	    glTexCoord2f(	0, 1 );
	    glVertex2d(	-1, 1 );
    
	    glTexCoord2f(	0, 0 );
	    glVertex2d(	-1, -1 );
    
	    glTexCoord2f(	1, 0 );
	    glVertex2d(	1, -1 );
    
	    glTexCoord2f(	1, 1 );
	    glVertex2d( 1, 1 );
    } glEnd();
    */

     glBegin(GL_QUADS);
    {
	    glTexCoord2f(	.25f, .75f );
	    glVertex2d(	-0.5, 0.5 );
    
	    glTexCoord2f(	.25f, .25f );
	    glVertex2d(	-0.5, -0.5);
    
	    glTexCoord2f(	.75f, .25f );
	    glVertex2d(	0.5, -0.5 );
    
	    glTexCoord2f(	.75f, .75f );
	    glVertex2d( 0.5, 0.5 );
    } glEnd();

  } glDisable(GL_TEXTURE_2D); 
}
