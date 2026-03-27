#include "../../al.h"
#include <math.h>

#define NUM_COLS 3
#define ARC_CY 70
#define ARC_R 42
#define ARC_START_DEG 40.0f
#define ARC_END_DEG 320.0f
#define ARC_STEP_DEG 2.0f
#define PI 3.14159265f

typedef struct {
  al_info_t info;
  const char *unit;
  const char *fmt;
} sensor_t;

static sensor_t sensors[] = {
    {AL_INFO_SENSOR_CO2,         "ppm", "%.0f"},
    {AL_INFO_SENSOR_TEMPERATURE, "\xc2\xb0""C",  "%.1f"},
    {AL_INFO_SENSOR_HUMIDITY,    "%",   "%.0f"},
    {AL_INFO_SENSOR_VOC,         "VOC", "%.0f"},
    {AL_INFO_SENSOR_NOX,         "NOx", "%.0f"},
    {AL_INFO_SENSOR_PRESSURE,    "hPa", "%.0f"},
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

static void draw_arc(int cx, int cy, int r) {
  float end = ARC_END_DEG;
  for (float a = ARC_START_DEG; a < end; a += ARC_STEP_DEG) {
    float a1 = a * (PI / 180.0f);
    float a2 = (a + ARC_STEP_DEG < end ? a + ARC_STEP_DEG : end) * (PI / 180.0f);
    int x1 = cx + (int)(r * sinf(a1) + 0.5f);
    int y1 = cy - (int)(r * cosf(a1) + 0.5f);
    int x2 = cx + (int)(r * sinf(a2) + 0.5f);
    int y2 = cy - (int)(r * cosf(a2) + 0.5f);
    al_line(x1, y1, x2, y2, 1, 2);
  }
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

    // arc outline
    draw_arc(cx, ARC_CY, ARC_R);

    // unit label above arc opening
    al_write(cx, ARC_CY - ARC_R - 12, 0, 8, 1, s->unit, AL_WRITE_ALIGN_CENTER);

    // value inside arc
    float val = al_info(s->info);
    char buf[16];
    snprintf(buf, sizeof(buf), s->fmt, val);
    al_write(cx, ARC_CY - 12, 0, 24, 1, buf, AL_WRITE_ALIGN_CENTER);
  }

  return 0;
}
