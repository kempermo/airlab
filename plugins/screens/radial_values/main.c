#include "../../al.h"
#include "arc_bitmap.h"
#include <math.h>

#define NUM_COLS 3
#define ARC_CY 70
#define PI 3.14159265f

// arc spans 280° visually; after 180° rotation: min at 220°, max at 140° (through top)
#define GAUGE_START_DEG 220.0f
#define GAUGE_SPAN_DEG  280.0f
#define GAUGE_LEN        10

typedef struct {
  al_info_t info;
  const char *unit;
  const char *fmt;
  float min_val;
  float max_val;
} sensor_t;

static sensor_t sensors[] = {
    {AL_INFO_SENSOR_CO2,          "ppm", "%.0f",  400.0f, 2000.0f},
    {AL_INFO_SENSOR_TEMPERATURE,  "\xc2\xb0""C",  "%.1f",   0.0f,   50.0f},
    {AL_INFO_SENSOR_HUMIDITY,     "%",   "%.0f",   0.0f,  100.0f},
    {AL_INFO_SENSOR_VOC,          "VOC", "%.0f",   0.0f,  500.0f},
    {AL_INFO_SENSOR_NOX,          "NOx", "%.0f",   0.0f,  200.0f},
    {AL_INFO_SENSOR_PRESSURE,     "hPa", "%.0f", 900.0f, 1100.0f},
};

static int find_sensor(const char *key, const char *def) {
  char value[32] = {0};
  al_config_get_s(key, value, sizeof(value));
  const char *v = strlen(value) > 0 ? value : def;
  if (strcmp(v, "co2") == 0) return 0;
  if (strcmp(v, "tmp") == 0) return 1;
  if (strcmp(v, "hum") == 0) return 2;
  if (strcmp(v, "voc") == 0) return 3;
  if (strcmp(v, "nox") == 0) return 4;
  if (strcmp(v, "prs") == 0) return 5;
  return 0;
}

int main() {

  int cols[NUM_COLS] = {
      find_sensor("col1", "co2"),
      find_sensor("col2", "voc"),
      find_sensor("col3", "tmp"),
  };

  al_clear(0);

  int col_w = AL_W / NUM_COLS;

  for (int i = 0; i < NUM_COLS; i++) {
    sensor_t *s = &sensors[cols[i]];
    int cx = i * col_w + col_w / 2;

    float val = al_info(s->info);

    // arc outline (pre-computed bitmap, opening at bottom)
    al_draw(cx - ARC_BITMAP_R, ARC_CY - ARC_BITMAP_R,
            ARC_BITMAP_SIZE, ARC_BITMAP_SIZE, 1, 180, arc_img, arc_mask);

    // gauge indicator line on the arc
    float ratio = (val - s->min_val) / (s->max_val - s->min_val);
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    float deg = GAUGE_START_DEG + ratio * GAUGE_SPAN_DEG;
    if (deg >= 360.0f) deg -= 360.0f;
    float rad = deg * (PI / 180.0f);
    int gx1 = cx + (int)((ARC_BITMAP_R - GAUGE_LEN) * sinf(rad) + 0.5f);
    int gy1 = ARC_CY - (int)((ARC_BITMAP_R - GAUGE_LEN) * cosf(rad) + 0.5f);
    int gx2 = cx + (int)(ARC_BITMAP_R * sinf(rad) + 0.5f);
    int gy2 = ARC_CY - (int)(ARC_BITMAP_R * cosf(rad) + 0.5f);
    al_line(gx1, gy1, gx2, gy2, 1, 2);

    // unit label below arc opening
    al_write(cx, ARC_CY + ARC_BITMAP_R - 12, 0, 8, 1, s->unit, AL_WRITE_ALIGN_CENTER);

    // value inside arc
    char buf[16];
    snprintf(buf, sizeof(buf), s->fmt, val);
    al_write(cx, ARC_CY - 12, 0, 24, 1, buf, AL_WRITE_ALIGN_CENTER);
  }

  return 0;
}
