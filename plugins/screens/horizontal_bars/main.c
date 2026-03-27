#include "../../al.h"

#define NUM_COLS 6
#define LINE_H 2
#define GAP 2
#define STRIPE_H (LINE_H + GAP)
#define BAR_TOP 20
#define BAR_BOT 108
#define BAR_AREA_H (BAR_BOT - BAR_TOP)
#define UNIT_Y 112

typedef struct {
  al_info_t info;
  const char *unit;
  const char *fmt;
  float min_val;
  float max_val;
} col_t;

static col_t cols[NUM_COLS] = {
    {AL_INFO_SENSOR_CO2,         "ppm", "%.0f",  400.0f, 2000.0f},
    {AL_INFO_SENSOR_VOC,         "VOC", "%.0f",    0.0f,  500.0f},
    {AL_INFO_SENSOR_TEMPERATURE, "\xc2\xb0""C",  "%.1f",    0.0f,   50.0f},
    {AL_INFO_SENSOR_HUMIDITY,    "%",   "%.0f",    0.0f,  100.0f},
    {AL_INFO_SENSOR_NOX,         "NOx", "%.0f",    0.0f,  200.0f},
    {AL_INFO_SENSOR_PRESSURE,    "hPa", "%.0f",  900.0f, 1100.0f},
};

int main() {
  al_clear(0);

  int col_w = AL_W / NUM_COLS;
  int bar_w = col_w - 6;
  int max_stripes = BAR_AREA_H / STRIPE_H;

  for (int i = 0; i < NUM_COLS; i++) {
    col_t *c = &cols[i];
    int x = i * col_w;

    // read value
    float val = al_info(c->info);
    char buf[16];
    snprintf(buf, sizeof(buf), c->fmt, val);

    // display unit
    al_write(x+col_w/2, UNIT_Y+4, 0, 14, 1, c->unit, AL_WRITE_ALIGN_CENTER);

    // normalize to [0,1] against fixed range
    float ratio = (val - c->min_val) / (c->max_val - c->min_val);
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    // draw horizontal stripes from bottom up
    int fill_stripes = (int)(ratio * max_stripes);
    for (int s = 0; s < fill_stripes; s++) {
      int y = BAR_BOT - (s + 1) * STRIPE_H + GAP;
      al_rect(x + 1, y, bar_w, LINE_H, 1, 0);
    }

    // display value
    int yPos = BAR_BOT - (fill_stripes + 1) * STRIPE_H + GAP - 16;
    al_write(x+col_w/2, yPos, 0, 16, 1, buf, AL_WRITE_ALIGN_CENTER);
  }

  return 0;
}
