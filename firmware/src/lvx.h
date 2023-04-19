#ifndef LVX_H
#define LVX_H

#include <lvgl.h>

#include "sig.h"

/* Wheel */

typedef struct {
  const char *format;
  uint16_t value;
  uint16_t min;
  uint16_t max;
  bool fixed;
  // ---
  lv_obj_t *_col;
  lv_obj_t *_up;
  lv_obj_t *_lbl;
  lv_obj_t *_down;
} lvx_wheel_t;

void lvx_wheel_create(lvx_wheel_t *wheel, lv_obj_t *parent);

/* Sign */

typedef struct {
  const char *title;
  const char *text;
  lv_align_t align;
  lv_coord_t offset;
  // ---
  lv_obj_t *_row;
  lv_obj_t *_title;
  lv_obj_t *_text;
} lvx_sign_t;

void lvx_sign_create(lvx_sign_t *sign, lv_obj_t *parent);

/* Helpers */

bool lvx_handle(sig_event_t event, bool focus);

void lvx_log_event(lv_event_t *event);

void lvx_style_set_pad(lv_style_t *style, lv_coord_t top, lv_coord_t bottom, lv_coord_t left, lv_coord_t right);

#endif  // LVX_H
