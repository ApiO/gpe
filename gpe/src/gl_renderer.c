#include "gl_renderer.h"

#include <GL/glfw.h>
#include <stdio.h>
#include <time.h>

void _print_diff(char *title, clock_t start, clock_t end, U32 ct)
{
   F64 ms = ((F64)(end - start) / CLOCKS_PER_SEC)*1000;
   printf("%s\t%.3f\t moy=%.6f\n", title, ms, ms/ct);
}

void gl_renderer_init (gpe_gl_renderer *r)
{
}

void gl_renderer_add (gpe_gl_renderer *r, gpe_gl_graphic_t graphic)
{
}

void gl_renderer_update (gpe_gl_renderer *r)
{
}

void gl_renderer_draw (gpe_gl_renderer *r)
{
}


void gl_renderer_free (gpe_gl_renderer *r)
{
}