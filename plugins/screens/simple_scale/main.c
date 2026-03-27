#include "../../al.h"

#define SCALE_Y 84
#define MAJOR_TICK_Y (SCALE_Y + 25)
#define MINOR_TICK_Y (SCALE_Y + 35)
#define LABEL_Y 95
#define LINE_WIDTH 2

typedef struct {
  al_info_t info;
  const char *unit;
  const char *fmt;
  float min_val;
  float max_val;
  float major_step;
  float minor_step;
} sensor_t;

static sensor_t sensors[] = {
    {AL_INFO_SENSOR_CO2,         "ppm", "%.0f",   0.0f, 2000.0f, 500.0f, 100.0f},
    {AL_INFO_SENSOR_TEMPERATURE, "°C",  "%.1f",   0.0f,   50.0f,  10.0f,   5.0f},
    {AL_INFO_SENSOR_HUMIDITY,    "%",   "%.0f",   0.0f,  100.0f,  25.0f,   5.0f},
    {AL_INFO_SENSOR_VOC,         "VOC", "%.0f",   0.0f,  500.0f, 100.0f,  50.0f},
    {AL_INFO_SENSOR_NOX,         "NOx", "%.0f",   0.0f,  200.0f,  50.0f,  10.0f},
    {AL_INFO_SENSOR_PRESSURE,    "hPa", "%.0f", 900.0f, 1100.0f,  50.0f,  10.0f},
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

static int val_to_x(float val, float min_val, float max_val) {
  float ratio = (val - min_val) / (max_val - min_val);
  if (ratio < 0.0f) ratio = 0.0f;
  if (ratio > 1.0f) ratio = 1.0f;
  return (int)(ratio * (AL_W - 1));
}

int main() {
  int sidx = find_sensor("sensor", "tmp");
  sensor_t *s = &sensors[sidx];

  float val = al_info(s->info);
  al_clear(0);

  // unit label (small, top center)
  al_write(AL_W / 2, 18, 0, 8, 1, s->unit, AL_WRITE_ALIGN_CENTER);

  // primary value (large, centered)
  char buf[16];
  snprintf(buf, sizeof(buf), s->fmt, val);
  al_write(AL_W / 2, 40, 0, 24, 1, buf, AL_WRITE_ALIGN_CENTER);

  // downward triangle marker
  int marker_x = AL_W /2 ;//val_to_x(val, s->min_val, s->max_val);
  int tri_y = 75;
  for (int row = 0; row < 6; row++) {
    int w = 11 - row * 2;
    al_rect(marker_x - w / 2, tri_y + row, w, 1, 1, 0);
  }

  // minor ticks
  int scale_x = val_to_x(val, s->min_val, s->max_val);

  float minor_increment = s->minor_step/2;

  if(strcmp(s->unit,"%")==0){ 
    minor_increment = s->minor_step/2.4;
  }

  if(strcmp(s->unit,"°C")==0){ 
    minor_increment = s->minor_step/4;
  }

  if(strcmp(s->unit,"VOC")==0){ 
    minor_increment = s->minor_step/4;
  }

  for (float v = s->min_val; v <= s->max_val + 0.001f; v += minor_increment) {
    int tx = val_to_x(v, s->min_val, s->max_val)+AL_W/2-scale_x;
    al_line(tx, MINOR_TICK_Y, tx, AL_H - 1, 1, LINE_WIDTH);
  }

  // major ticks + labels (skip min_val label)
  for (float v = s->min_val + s->major_step; v <= s->max_val + 0.001f; v += s->major_step) {
    int tx = val_to_x(v, s->min_val, s->max_val)+AL_W/2-scale_x;
    al_line(tx, MAJOR_TICK_Y, tx, AL_H - 1, 1, LINE_WIDTH);
    char label[12];
    snprintf(label, sizeof(label), "%.0f", v);
    al_write(tx, LABEL_Y, 0, 8, 1, label, AL_WRITE_ALIGN_CENTER);
  }

  return 0;
}
