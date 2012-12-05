#include "fonts.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SOIL\SOIL.h>
/*

int fonts_height = 256;
int fonts_width = 256;
int fonts_chanels = 1;
unsigned char * fonts_imgData;

*/

void fonts_print(char *text);
int fonts_loadfont(char *filepath);
int fonts_parseline(char *line);
int  fonts_getKeyValue(char * line, char * key);
char *fonts_getPathValue(char * line, char * key);
void fonts_substring(int start, int stop, const char *text);
int  fonts_indexof(char *str1, char *str2);

Charset BMFont;

int fonts_indexof(char *str1, char *str2)
{
  char *tmp = strstr(str1, str2);
  return tmp - str1;
}

void fonts_substring(int start, int stop, const char *src, char *dst)
{
   int count = stop - start;
   sprintf(dst, "%.*s", count, src + start);
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

int fonts_loadfont(char *filepath)
{
  char line_buffer[BUFSIZ];
  FILE *file = fopen(filepath, "r");
  if (!file) {
      printf("Couldn't open file \"%s\" for reading.\n", filepath);
      return -1;
  }

  int succeed = 1;
  while (fgets(line_buffer, sizeof(line_buffer), file) && succeed == 1) {
    succeed = fonts_parseline(line_buffer);
  }

  fclose(file);

  return succeed;
}

int fonts_parseline(char *line)
{
  CharDescriptor charDesc;
  int id ;
  char *file;

  //int i;
  if (fonts_indexof (line, "common ") > -1)
  {
    //printf("common\n");
    
    BMFont.LineHeight = fonts_getKeyValue(line, "lineHeight=");
    BMFont.Base = fonts_getKeyValue(line, "base=");
    BMFont.Width = fonts_getKeyValue(line, "scaleW=");
    BMFont.Height = fonts_getKeyValue(line, "scaleH=");
    BMFont.Pages = fonts_getKeyValue(line, "pages=");
  }
  else if (fonts_indexof(line, "page ") > -1)
  {
    char *fileTemp = "C:\\Users\\utilisateur\\Desktop\\BMFont ressources\\QuicksandBook_0.png";
    file = fonts_getPathValue(line, "file=");
    id = fonts_getKeyValue(line, "id=");
    //printf("%s\n", file);

    BMFont.tex_2d = SOIL_load_OGL_texture(fileTemp, //file, 
      SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO);

    if( !BMFont.tex_2d )
    {
      printf("Couldn't open file \"%s\" for reading.\n", fileTemp);
      free(file);
      return -1;
    } 
    
    free(file);
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

void fonts_print(char *text)
{
  int i, txtLen;
  txtLen = strlen(text);
  
  for (i=0; i < txtLen; i++)
  {
    //In C, you can use the char data type just like a number and it will automatically convert it to the ASCII value
    // ex: int i = '!' <=>  i = 33;
    //printf("[char index]=%d, [char]=\'%c\', [ascii code]=%d, [FONT x]=%d, [FONT y]=%d\n", i, text[i], text[i], BMFont.Chars[text[i]].x , BMFont.Chars[text[i]].y);
  }
}

void fonts_bitmap (char *text, int x, int y)
{
}

void fonts_outline (char *text)
{
}

void fonts_texture_mapped_outline (char *text)
{
}

void fonts_2d_texture (char *text)
{
}

void fonts_foo_load(void)
{ 
  int state;
  state = fonts_loadfont("C:\\Users\\utilisateur\\Desktop\\BMFont ressources\\QuicksandBook.fnt");

  if(state == 1)
  {
    fonts_print("ASCII");
  }
  //fonts_imgData = SOIL_load_image("C:\\Users\\utilisateur\\Desktop\\BMFont ressources\\QuicksandBook.png", &fonts_height, &fonts_width, &fonts_chanels, 0);
}

void fonts_foo_draw(void)
{

}

void fonts_foo_free(void)
{
    //SOIL_free_image_data(fonts_imgData);
}