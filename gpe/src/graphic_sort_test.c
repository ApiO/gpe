#include "graphic_sort_test.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h> 

#include "graphic_system.h"
#include "gpr_memory.h"
#include "gpr_array.h"

const int ELEMENTS = 4;

//void _sort(graphic_array *arr);
void _system_init(graphic_system *system);
//void _print_array (char * title, graphic_array *arr, I32 count);

int _graphic_sort_random(int min, int max) 
{ 
  return (min + (rand() % (max-min+1))); 
} 

void _system_init(graphic_system *system)
{
  int i;
  U64 elemet_id;
  gpe_graphic *graphic;
  GLuint tex_id;
  
  graphic_system_init(system);

  for(i=0; i < ELEMENTS; i++)
  {
    tex_id = (GLuint)_graphic_sort_random(0,3);
    
    elemet_id = graphic_system_add(system, tex_id);

    graphic = graphic_system_lookup(system, elemet_id);

    graphic->tex_id = tex_id;
    graphic->z = _graphic_sort_random(0,4);
  }
}

/*
void _sort(graphic_array *arr)
{
  const int MAX_LEVELS = 64;
  int i = 0;
  gpe_graphic *beg[MAX_LEVELS], *end[MAX_LEVELS], *left, *right, piv, *swap;

  beg[0] = arr->data[0];
  end[0] = arr->data[arr->size-1];
  while (i>=0)
  {
    left = beg[i]; 
    right = end[i]-1;

    if (left->z < right->z && left->tex_id < right->tex_id)
    {
      //piv = arr[
    }
  }
}
  */

void foo(void)
{
/*
  graphic_system system;

  gpr_memory_init(4*1024*1024);
  
  srand (time (NULL)); 

  _system_init(&system);  

  _print_array("before sorting", &system.graphics, system.graphics.size);
  //sort
  _sort(&system.graphics);
  _print_array("after sorting", &system.graphics, system.graphics.size);

  graphic_system_free(&system);
  gpr_memory_shutdown();
  */
}
/*
void _print_array (char *title, graphic_array *arr, I32 count)
{
  gpe_graphic *graphic;

  int j;
  printf("%s\n", title);
  for ( j = 0; j < count; j++)
  {
    graphic = arr->data[j];
    //graphic = arr->data[j]->graphic;
    printf("z = %d | tex_id = %d\n", graphic->z, graphic->tex_id);
  }
}
*/

void _quickSort(int *arr, int elements) {

  const int MAX_LEVELS = 64;

  int  piv, beg[MAX_LEVELS], end[MAX_LEVELS], i=0, L, R, swap ;

  beg[0]=0; end[0]=elements;
  while (i>=0) 
  {
    L=beg[i]; R=end[i]-1;
    if (L<R) 
    {
      piv=arr[L];
      while (L<R) 
      {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; 
      }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L;
      if (end[i]-beg[i]>end[i-1]-beg[i-1]) 
      {
        swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
        swap=end[i]; end[i]=end[i-1]; end[i-1]=swap; 
      }
    }
    else 
    {
      i--; 
    }
  }
}

void foo_hgkghjk(void)
{
  const int range = 100;
  int i;
  int lst[range];
  for(i=1; i<range; i++)
  {
    lst[i] = rand();
  }

  _quickSort( lst, range);

  for(i=1; i<range; i++)
  {
    printf("%d\n", lst[i]);
  }
  
}