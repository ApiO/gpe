#include "fonts.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
#include <SOIL\SOIL.h>

int fonts_height = 256;
int fonts_width = 256;
int fonts_chanels = 1;
unsigned char * fonts_imgData;

*/
void fonts_parseline(char line[]);
void fonts_parsefont(char *filepath);
int fonts_indexof(char *str1, char *str2);
int fonts_getvalue(char * line, char * key);
void fonts_substring(int start, int stop, const char *text);

char *substring(size_t start, size_t stop, const char *src, char *dst, size_t size)
{
   int count = stop - start;
   if ( count >= --size )
   {
      count = size;
   }
   sprintf(dst, "%.*s", count, src + start);
   return dst;
}

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

int fonts_getvalue(char *line, char *key)
{
  char *result;
  char *tmp;
  int key_start_index, value_start_index, value_end_index;

  key_start_index = fonts_indexof(line, key);
  value_start_index = key_start_index + strlen(key);

  
  tmp = (char *) malloc((strlen(line)-value_start_index)*sizeof(char));
  fonts_substring(value_start_index, strlen(line)-value_start_index, line, tmp);
  
  value_end_index = fonts_indexof(tmp, " ");
  if (value_end_index < 0) //si c'est le dernier @
  {
    value_end_index = fonts_indexof(tmp, "\0");
  }

  result = (char *) malloc((value_end_index+1)*sizeof(char));
  fonts_substring(0, value_end_index, tmp, result);
  
  printf("KEY:\"%s\" VALUE:\"%s\"\n", key, result);

  free(result);
  free(tmp); //crash uniquement avec la clef : "pages=" quand valeure n'a que 1 char

  return -1;
}

void fonts_parsefont(char *filepath)
{
  char line_buffer[BUFSIZ];
  FILE *file = fopen(filepath, "r");
  if (!file) {
      printf("Couldn't open file \"%s\" for reading.\n", filepath);
      return;
  }

  while (fgets(line_buffer, sizeof(line_buffer), file)) {
    //printf("%s", line_buffer);
    fonts_parseline(line_buffer);
  }

  fclose(file);
}

void fonts_parseline(char *line)
{
  //int i;
  if (fonts_indexof (line, "common ") > -1)
  {
    printf("common\n");
    fonts_getvalue(line, "lineHeight=");
    fonts_getvalue(line, "base=");
    fonts_getvalue(line, "scaleW=");
    fonts_getvalue(line, "scaleH=");
    fonts_getvalue(line, "pages=");
  }
  else if (fonts_indexof(line, "char ") > -1)
  {
    //printf("char\n");
  }
  /*
  for(i = 0; i<BUFSIZ; i++)
  {
    if(line[i] =='\n')
    {
      continue;
    }
  }
  */
}


void fonts_bitmap (char *text, int x, int y)
{
}

void fonts_outline (char *text)
{
}

void fonts_texture_mapped_outline (char * text)
{
}

void fonts_2d_texture (char * text)
{
}

void fonts_foo_load(void)
{ 
  fonts_parsefont("QuicksandBook.fnt");
  //fonts_imgData = SOIL_load_image("C:\\Users\\utilisateur\\Desktop\\BMFont ressources\\QuicksandBook.png", &fonts_height, &fonts_width, &fonts_chanels, 0);
}

void fonts_foo_draw(void)
{

}

void fonts_foo_free(void)
{
    //SOIL_free_image_data(fonts_imgData);
}