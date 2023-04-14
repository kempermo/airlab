#include <lvgl.h>

void gfx_init();

void gfx_begin(bool flush, bool invert);
void gfx_end();

lv_group_t* gfx_get_group();
