#include "fonts.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SOIL\SOIL.h>


int   fonts_parseline(char *path, char *line);
int   fonts_getKeyValue(char * line, char *key);
int   fonts_indexof(char *str1, char *str2);
int   fonts_last_indexof(char *str1, char *str2);
int   fonts_getStringWidth(charset *font, char *text);
int   fonts_renderString(char *text);
void  fonts_substring(int start, int stop, char *text, char *out);
char *fonts_getPathValue(char * line, char *key);
char *fonts_getFilepath(char *filepath);
char *fonts_strAppend(char *str1, char *str2);

charset BMFont;

int fonts_indexof(char *str1, char *str2)
{
  char *tmp = strstr(str1, str2);
  return tmp - str1;
}

int fonts_last_indexof(char *str, char chr)
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

void fonts_substring(int start, int stop, char *src, char *out)
{
   int count = stop - start;
   sprintf(out, "%.*s", count, src + start);
}

int fonts_getKeyValue(char *line, char *key)
{
  int value;
  char *result;
  char *tmp;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = fonts_indexof(line, key);
  value_start_index = key_start_index + strlen(key);

  
  tmp = (char *) malloc((strlen(line)-value_start_index+1)*sizeof(char));
  fonts_substring(value_start_index, strlen(line)-value_start_index, line, tmp);
  
  value_end_index = fonts_indexof(tmp, " ");
  if (value_end_index < 0)
  {
    value_end_index = fonts_indexof(tmp, "\0");
  }

  result = (char *) malloc((value_end_index+1)*sizeof(char));
  fonts_substring(0, value_end_index, tmp, result);
  
  value = atoi(result);

  free(result);
  free(tmp);

  //printf("KEY:\"%s\" VALUE:\"%d\"\n", key, value);

  return value;
}

char *fonts_getPathValue(char *line, char *key)
{
  char *tmp;
  char *result;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = fonts_indexof(line, key);
  value_start_index = key_start_index + strlen(key) + 1;

  tmp = (char *) malloc((strlen(line)-value_start_index+1)*sizeof(char));
  fonts_substring(value_start_index, strlen(line), line, tmp);
  
  value_end_index = fonts_indexof(tmp, "\"");

  result = (char *) malloc((value_end_index+1)*sizeof(char));
  fonts_substring(0, value_end_index, tmp, result);
  
  free(tmp);
  return result;
}

int fonts_getStringWidth(charset *font, char *text)
{
  int w, l, i;
  w = 0;
  l = strlen(text);
	for (i=0;i < l; i++) {
		w += font->Chars[text[i]].Width;
	}
	return w+l;
}

int fonts_parseline(char *path, char *line)
{
  charDescriptor charDesc;
  int id ;
  char *filename;
  char *imagepath;

  if (fonts_indexof (line, "common ") > -1)
  {
    BMFont.LineHeight = fonts_getKeyValue(line, "lineHeight=");
    BMFont.Base = fonts_getKeyValue(line, "base=");
    BMFont.Width = fonts_getKeyValue(line, "scaleW=");
    BMFont.Height = fonts_getKeyValue(line, "scaleH=");
    BMFont.Pages = fonts_getKeyValue(line, "pages=");
  }
  else if (fonts_indexof(line, "page ") > -1)
  {    
    filename = fonts_getPathValue(line, "file=");
    imagepath = fonts_strAppend(path, filename);
    id = fonts_getKeyValue(line, "id=");
    
    printf("Loading image file:\n\t\"%s\"\n", imagepath);
    BMFont.tex_2d[id] = SOIL_load_OGL_texture(imagepath, //file, 
      SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO);

    if( !BMFont.tex_2d[id] )
    {
      printf("Couldn't open file \"%s\" for reading.\n", imagepath);
      free(filename);
      free(imagepath);
      return -1;
    } 
    
    free(filename);
    free(imagepath);
  }
  else if (fonts_indexof(line, "char ") > -1)
  {
    //printf("char\n");
    id = fonts_getKeyValue(line, "id=");
    charDesc.x = fonts_getKeyValue(line, "x=");
    charDesc.y = fonts_getKeyValue(line, "y=");
    charDesc.Width = fonts_getKeyValue(line, "width=");
    charDesc.Height = fonts_getKeyValue(line, "height=");
    charDesc.XOffset = fonts_getKeyValue(line, "xoffset=");
    charDesc.YOffset = fonts_getKeyValue(line, "yoffset=");
    charDesc.XAdvance = fonts_getKeyValue(line, "xadvance=");
    charDesc.Page = fonts_getKeyValue(line, "page=");
    charDesc.Chnl = fonts_getKeyValue(line, "chnl=");

    BMFont.Chars[id] = charDesc;
  }
  return 1;
}

void fonts_print(char *text, int x, int y)
{
  int i, txtLen;
  txtLen = strlen(text);
  
  printf("text:\t\t\"%s\"\n", text);
  printf("text width:\t%dpx\n", fonts_getStringWidth(&BMFont, text));
  
  printf("analysing text:\n");
  for (i=0; i < txtLen; i++)
  {
    //In C, you can use the char data type just like a number and it will automatically convert it to the ASCII value
    // ex: int i = '!' <=>  i = 33;
    printf("\t[char index]=%d, [char]=\'%c\', [ascii code]=%d, [FONT x]=%d, [FONT y]=%d\n", i, text[i], text[i], BMFont.Chars[text[i]].x , BMFont.Chars[text[i]].y);
  }
}

int fonts_load(char *filepath)
{ 
  int state = 1;
  char line_buffer[BUFSIZ];
  char *path;
  FILE *file = fopen(filepath, "r");

  if (!file) {
      printf("Couldn't open file \"%s\" for reading.\n", filepath);
      return -1;
  }

  printf("Loading font:\n\t\"%s\"\n", filepath);
  path = fonts_getFilepath(filepath);
  while (fgets(line_buffer, sizeof(line_buffer), file) && state == 1) {
    state = fonts_parseline(path, line_buffer);
  }
  fclose(file);
  free(path);

  return state;
}

char *fonts_strAppend(char *str1, char *str2)
{
  char *s = (char *)malloc((strlen(str1) + strlen(str2) +1)*sizeof(char));
  sprintf(s, "%s%s", str1, str2);
  return s;
}

int fonts_load(void)
{
  return fonts_load("C:\\Users\\utilisateur\\Desktop\\BMFont ressources\\QuicksandBook.fnt");
}

char *fonts_getFilepath(char *filepath)
{
  char *path;
  int index;
  index = fonts_last_indexof(filepath,'\\');

  if(index != NULL && index > 0)
  { 
    path = (char *) malloc(index+2);
    fonts_substring(0, index+1, filepath, path);
  }
  else
  {
    path = (char *) malloc(strlen(path));
    strcpy(path, path);
  }
  return path;
}

void fonts_free(void)
{
  //SOIL_free_image_data(BMFont.tex_2d);
}