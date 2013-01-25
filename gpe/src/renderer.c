#include "renderer.h"

#if _WIN32
  #include <windows.h>
#endif
#include <GL\GL.h>

#include "rsx_mngr_temp.h"

typedef struct
{
  U64 transform; 
  GLuint tex_id;
  GLuint cmd;
} gpe_render_item_t;

typedef gpr_array_t(gpe_render_item_t) render_buffer;

void _renderer_scene_to_render_item(gpe_scene_item_t *s, gpe_render_item_t *r);

void renderer_draw(graphic_buffer *gb)
{
  unsigned int i;
  gpr_tmp_allocator_2048_t ta;
  gpr_allocator_t *a = (gpr_allocator_t*)&ta;
  render_buffer rb;

  gpr_tmp_allocator_init(a, 2048);
  gpr_array_init(gpe_render_item_t, &rb, a);

  //feed du render buffer
  for(i=0; i< gb->size; i++)
  {
    gpe_render_item_t r;
    _renderer_scene_to_render_item(&gb->data[i], &r);
    gpr_array_push_back(gpe_render_item_t, &rb, r);
  }

  //creation des render context (parallelisable)

  //Draw

  //clean
  gpr_array_destroy(&rb);
  gpr_tmp_allocator_destroy(a);
}

void _renderer_scene_to_render_item(gpe_scene_item_t *s, gpe_render_item_t *r)
{
  //trans data de s vers r
  //recu des data ds rsx_mngr_temp
}