#include "font_system_test.h"

#include "window_manager.h"
#include "font_system.h"

#define HEIGHT 900
#define WIDTH  1200

typedef struct
{
  U64 id;
  gpe_text_align align;
  gpe_text_dock dock;
} tmp_fs_txt;
typedef gpr_array_t(tmp_fs_txt) tmp_txt;

wchar_t *sample_text =  L"abcdefghijklmnopqrstuvwxyz\n0123456789\nABCDEFGHYJKLMNOPQRSTUVWXYZ\n,;:!ù*$^=)àç_è-('\"é&\n?./§%µ£¨+°\n~#{[|`\\^@]}¤";
              
char *font1 = "..\\..\\src\\ressources\\QuicksandBold.14\\QuicksandBold.14.fnt";
char *font_key1 = "QuicksandBold.14";
char *font2 = "..\\..\\src\\ressources\\QuicksandBold.24\\QuicksandBold.24.fnt";
char *font_key2 = "QuicksandBold.24";
char *font3 = "..\\..\\src\\ressources\\QuicksandBook.32\\QuicksandBook.32.fnt";
char *font_key3 = "QuicksandBook.32";


#define ITEM_COUNT 5

void font_system_test_foo()
{
  int i;
  window_manager w;
  tmp_txt sample;
  gpr_memory_init(4*1024*1024);

  gpr_array_init(tmp_fs_txt, &sample, gpr_default_allocator);
  gpr_array_reserve(tmp_fs_txt, &sample, ITEM_COUNT*sizeof(tmp_fs_txt));


  window_manager_init(&w, "FONT_SYSTEM debug", HEIGHT, WIDTH);
  w.display_axes = true;
  
  font_system_init(gpr_default_allocator);
  
  font_system_load_font(font_key1, font1);
  font_system_load_font(font_key2, font2);
  font_system_load_font(font_key3, font3);
  
  tmp_fs_txt item;
  item.id = font_system_text_init(font_key1);
  item.dock = DOCK_TEXT_TOP_LEFT;
  item.align= ALIGN_TEXT_LEFT;
  gpr_array_push_back(tmp_fs_txt, &sample, item);
  font_system_text_set(item.id, sample_text, item.align);

  item.id = font_system_text_init(font_key2);
  item.dock = DOCK_TEXT_BOTTOM_LEFT;
  item.align= ALIGN_TEXT_LEFT;
  gpr_array_push_back(tmp_fs_txt, &sample, item);
  font_system_text_set(item.id, sample_text, item.align);

  item.id = font_system_text_init(font_key3);
  item.dock = DOCK_TEXT_CENTER;
  item.align= ALIGN_TEXT_CENTER;
  gpr_array_push_back(tmp_fs_txt, &sample, item);
  font_system_text_set(item.id, sample_text, item.align);

  item.id = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  item.dock = DOCK_TEXT_TOP_RIGHT;
  item.align= ALIGN_TEXT_RIGHT;
  gpr_array_push_back(tmp_fs_txt, &sample, item);
  font_system_text_set(item.id, sample_text, item.align);

  item.id = font_system_text_init(FSYS_DEFAULT_FONT_NAME);
  item.dock = DOCK_TEXT_BOTTOM_RIGHT;
  item.align= ALIGN_TEXT_RIGHT;
  gpr_array_push_back(tmp_fs_txt, &sample, item);
  font_system_text_set(item.id, sample_text, item.align);


  while(w.running)
  {
    window_manager_clear(&w);
    
    for(i=0; i<ITEM_COUNT; i++)
    {
      tmp_fs_txt *t = gpr_array_begin(&sample)+i;
      font_system_text_print(t->id, 0, 0, t->dock, HEIGHT, WIDTH);
    }

    window_manager_swapBuffers(&w);
  }

  
  gpr_array_destroy(&sample);
  font_system_free();
  window_manager_free(&w);
  gpr_memory_shutdown();
}
