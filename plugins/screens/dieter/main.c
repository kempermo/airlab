#include "../../al.h"

#define NUM_ROWS 3
#define ROW_H (AL_H / NUM_ROWS)   // 42
#define SCALE_W 270               // tick ruler width
#define TICK_STEP 7               // pixels between ticks
#define BOX_PAD_X 4               // horizontal padding inside value box
#define BOX_PAD_Y 2               // vertical padding for box within row
#define UNIT_X (AL_W - 15)         // unit label right edge

typedef struct {
  al_info_t info;
  const char *unit;
  const char *fmt;
  float min_val;
  float max_val;
} row_t;

static row_t groups[2][NUM_ROWS] = {
    {
        {AL_INFO_SENSOR_CO2,         "ppm", "%.0f",   0.0f, 2000.0f},
        {AL_INFO_SENSOR_VOC,         "VOC", "%.0f",   0.0f,  500.0f},
        {AL_INFO_SENSOR_TEMPERATURE, "\xc2\xb0""C",  "%.1f",   0.0f,   50.0f},
    },
    {
        {AL_INFO_SENSOR_NOX,         "NOx", "%.0f",   0.0f,  200.0f},
        {AL_INFO_SENSOR_HUMIDITY,    "%RH", "%.1f",   0.0f,  100.0f},
        {AL_INFO_SENSOR_PRESSURE,    "hPa", "%.0f", 900.0f, 1100.0f},
    },
};

int main() {
  // read group config
  char grp[4] = {0};
  al_config_get_s("group", grp, sizeof(grp));
  int g = (grp[0] == 'b') ? 1 : 0;
  row_t *rows = groups[g];

  al_clear(0);

  for (int ri = 0; ri < NUM_ROWS; ri++) {
    row_t *r = &rows[ri];
    int ry = ri * ROW_H;
    int tick_top = ry + BOX_PAD_Y+5;
    int tick_bot = ry + ROW_H - BOX_PAD_Y-5;

    // read value
    float val = al_info(r->info);
    float ratio = (val - r->min_val) / (r->max_val - r->min_val);
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    int val_x = (int)(ratio * SCALE_W);

    // format value string
    char buf[16];
    snprintf(buf, sizeof(buf), r->fmt, val);

    // compute box dimensions
    int box_w = (int)strlen(buf) * 9 + BOX_PAD_X * 2+6;
    if (box_w < 22) box_w = 22;
    int box_h = tick_bot+20 - tick_top-20;
    int box_x = val_x - box_w / 2;
    if (box_x < 0) box_x = 0;
    if (box_x + box_w > SCALE_W) box_x = SCALE_W - box_w;

    // draw ticks across full row
    for (int tx = 0; tx <= SCALE_W; tx += TICK_STEP) {
      if(tx%5 == 0)
        al_line(tx, tick_top+5, tx, tick_bot-5, 1, 1);
      else
        al_line(tx, tick_top+10, tx, tick_bot-10, 1, 1);
    }

    // draw inverted value box
    al_rect(box_x, tick_top+3, box_w, 22, 1, 0);

    // draw value text in white inside box
    int text_y = tick_top + (box_h - 16) / 2;
    al_write(box_x + box_w / 2, text_y+1, 0, 16, 0, buf, AL_WRITE_ALIGN_CENTER);

    // draw unit label (font 8, vertically centered, right-aligned)
    int unit_y = ry + (ROW_H - 8) / 2;
    al_write(UNIT_X, unit_y, 0, 8, 1, r->unit, AL_WRITE_ALIGN_CENTER);
  }

  return 0;
}
