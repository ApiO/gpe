#include "graphic_system.h"

#include "gpr_memory.h"

#include <stdio.h>

typedef struct 
{
  U32 index;
  F64 weight;
} gpe_weight_t;

typedef struct 
{
	void *left, *right;
	int depth;
} gpe_isort_t;

void _graphic_system_mergesort(size_t n, gpe_weight_t array[]);
void _graphic_system_insertsort(gpe_weight_t *s, gpe_weight_t *t);
void _graphic_system_combsort(size_t n, gpe_weight_t a[]);
void _graphic_system_introsort(size_t n, gpe_weight_t array[]);
void _graphic_system_bind_gl_item(gpe_graphic *graphic, gpe_gl_graphic *gl_graphic);

void graphic_system_init (graphic_system *system)
{
  gpr_idlut_init(gpe_graphic, &(system->table), gpr_default_allocator);
}

//return idlut id of the tex within the system
U64 graphic_system_add (graphic_system *system, GLuint tex_id)
{
  gpe_graphic graphic;
  graphic.tex_id = tex_id;
  return gpr_idlut_add(gpe_graphic, &system->table, &graphic);
}

void graphic_system_remove (graphic_system *system, U64 graphic_id)
{
  gpr_idlut_remove(gpe_graphic, &system->table, graphic_id);
}

gpe_graphic* graphic_system_lookup (graphic_system *system, U64 graphic_id)
{
  return gpr_idlut_lookup(gpe_graphic, &system->table, graphic_id);
}

void graphic_system_free (graphic_system *system)
{
  gpr_idlut_destroy(gpe_graphic, &system->table);
}

void graphic_system_update (graphic_system *system)
{
  if (system->table.num_items <= 1) return;
  
  {
    U32 i;
    I32 max_z;
    U64 max_t;
    gpe_graphic *graphic;
    gpr_array_t(gpe_weight_t) w;
    
    gpr_array_init(gpe_weight_t, &w, gpr_default_allocator);

    max_z = 0; max_t = 0;
    for( i=0; i<system->table.num_items; i++)
    {
      graphic = gpr_idlut_begin(gpe_graphic, &system->table) + i;
      if (true) //if on cam
      {
        {
        gpe_weight_t item;
        item.index = i;

        if(max_z < graphic->z) max_z = graphic->z;
        if(max_t < graphic->tex_id) max_t = graphic->tex_id;

        gpr_array_push_back(gpe_weight_t, &w, item);
        }
      }
    }

    //avoid to divid by 0
    if (max_z==0) max_z = 1;
    
    for( i=0; i<w.size; i++)
    {
      graphic = gpr_idlut_begin(gpe_graphic, &system->table) + i;
      w.data[i].weight = ((F64)graphic->z / max_z)*max_t + 
                         (F64)graphic->tex_id / max_t;
    }

    //sort
    //_graphic_system_introsort(w.size, w.data);
    _graphic_system_mergesort(w.size, w.data);
    /*
    //print sort
    for( i=0; i<w.size; i++)
    {
      graphic = gpr_idlut_begin(gpe_graphic, &system->table) + w.data[i].index;
      printf("id= %d\tz= %d\tex_id= %d\twidth= %f.5\n", graphic->dev, graphic->z, graphic->tex_id, w.data[i].weight);
    }
    */
    //push graph to renderer
    for( i=0; i<w.size; i++)
    {
      gpe_gl_graphic gl_graphic;
      graphic = gpr_idlut_begin(gpe_graphic, &system->table) + w.data[i].index;
      _graphic_system_bind_gl_item(graphic, &gl_graphic);
      gl_renderer_add(&system->renderer, gl_graphic);
    }

    gpr_array_destroy(&w);
  }
}

void _graphic_system_bind_gl_item(gpe_graphic *graphic, gpe_gl_graphic *gl_graphic)
{
  gl_graphic->tex_id = graphic->tex_id;
  gl_graphic->x = graphic->x;
  gl_graphic->y = graphic->y;
  gl_graphic->w = graphic->w;
  gl_graphic->h = graphic->h;
  gl_graphic->scale = graphic->scale;
  gl_graphic->r = graphic->r;
  gl_graphic->a = graphic->a;
  gl_graphic->shear_x = graphic->shear_x;
  gl_graphic->shear_y = graphic->shear_y;
  //gl_graphic->z = graphic->z;
}

void graphic_system_render (graphic_system *system)
{
  gl_renderer_draw(&system->renderer);
}

void _graphic_system_combsort(size_t n, gpe_weight_t a[])
{
	const double shrink_factor = 1.2473309501039786540366528676643;
	int do_swap;
	size_t gap = n;
	gpe_weight_t tmp, *i, *j;
	do {
		if (gap > 2) 
    {
			gap = (size_t)(gap / shrink_factor);
			if (gap == 9 || gap == 10) gap = 11;
		}
		do_swap = 0;
		for (i = a; i < a + n - gap; ++i) 
    {
			j = i + gap;
			if ((*j).weight < (*i).weight) 
      {
				tmp = *i; *i = *j; *j = tmp;
				do_swap = 1;
			}
		}
	} while (do_swap || gap > 2);
	if (gap != 1) _graphic_system_insertsort(a, a + n);
}

void _graphic_system_insertsort(gpe_weight_t *s, gpe_weight_t *t)
{
	gpe_weight_t *i, *j, swap_tmp;
	for (i = s + 1; i < t; ++i)
		for (j = i; j > s && (*j).weight < (*(j-1)).weight; --j)
			swap_tmp = *j; *j = *(j-1); *(j-1) = swap_tmp;
}

void _graphic_system_introsort(size_t n, gpe_weight_t a[])
{
	int d;
	gpe_isort_t *top, *stack;
	gpe_weight_t rp, swap_tmp;
	gpe_weight_t *s, *t, *i, *j, *k;

	if (n < 1) return;
	else if (n == 2) 
  {
		if (a[1].weight < a[0].weight)
      { swap_tmp = a[0]; a[0] = a[1]; a[1] = swap_tmp; }
		return;
	}
	for (d = 2; 1ul<<d < n; ++d);
	stack = (gpe_isort_t*)malloc(sizeof(gpe_isort_t) * ((sizeof(size_t)*d)+2));
	top = stack; s = a; t = a + (n-1); d <<= 1;
	while (1) 
  {
		if (s < t) 
    {
			if (--d == 0) 
      {
				_graphic_system_combsort(t - s + 1, s);
				t = s;
				continue;
			}
			i = s; j = t; k = i + ((j-i)>>1) + 1;
			if ((*k).weight < (*i).weight) 
      {
				if ((*k).weight < (*j).weight) k = j;
			} 
      else 
        k = (*j).weight < (*i).weight ? i : j;
			rp = *k;
			if (k != t) { swap_tmp = *k; *k = *t; *t = swap_tmp; }
			for (;;) 
      {
				do ++i; while ((*i).weight < (rp).weight);
				do --j; while (i <= j && (rp).weight < (*j).weight);
				if (j <= i) break;
				swap_tmp = *i; *i = *j; *j = swap_tmp;
			}
			swap_tmp = *i; *i = *t; *t = swap_tmp;
			if (i-s > t-i) 
      {
				if (i-s > 16)
          { top->left = s; top->right = i-1; top->depth = d; ++top; }
				s = t-i > 16? i+1 : t;
			} 
      else 
      {
				if (t-i > 16)
          { top->left = i+1; top->right = t; top->depth = d; ++top; }
				t = i-s > 16? i-1 : s;
			}
		} 
    else 
    {
			if (top == stack) 
      {
				free(stack);
				_graphic_system_insertsort(a, a+n);
				return;
			} 
      else 
      {
        --top; s = (gpe_weight_t*)top->left;
        t = (gpe_weight_t*)top->right;
        d = top->depth;
      }
		}
	}
}

void _graphic_system_mergesort(size_t n, gpe_weight_t array[])
{
	gpe_weight_t *a2[2], *a, *b;
	int curr, shift;

	a2[0] = array;
	a2[1] = (gpe_weight_t*)malloc(sizeof(gpe_weight_t) * n);
	for (curr = 0, shift = 0; (1ul<<shift) < n; ++shift)
  {
		a = a2[curr]; b = a2[1-curr];
		if (shift == 0) {
			gpe_weight_t *p = b, *i, *eb = a + n;
			for (i = a; i < eb; i += 2)
      {
				if (i == eb - 1) *p++ = *i;
				else {
					if ((*(i+1)).weight < (*i).weight)
          {
						*p++ = *(i+1); *p++ = *i;
					} else {
						*p++ = *i; *p++ = *(i+1);
					}
				}
			}
		} else {
			size_t i, step = 1ul<<shift;
			for (i = 0; i < n; i += step<<1) {
				gpe_weight_t *p, *j, *k, *ea, *eb;
				if (n < i + step) {
					ea = a + n; eb = a;
				} else {
					ea = a + i + step;
					eb = a + (n < i + (step<<1)? n : i + (step<<1));
				}
				j = a + i; k = a + i + step; p = b + i;
				while (j < ea && k < eb) {
					if ((*k).weight < (*j).weight) *p++ = *k++;
					else *p++ = *j++;
				}
				while (j < ea) *p++ = *j++;
				while (k < eb) *p++ = *k++;
			}
		}
		curr = 1 - curr;
	}
	if (curr == 1) {
		gpe_weight_t *p = a2[0], *i = a2[1], *eb = array + n;
		for (; p < eb; ++i) *p++ = *i;
	}
	free(a2[1]);
}