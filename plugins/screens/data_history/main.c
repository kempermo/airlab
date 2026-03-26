#include "../../al.h"

#define MAX_BARS 59
#define BAR_W 4
#define BAR_GAP 1
#define BAR_STEP (BAR_W + BAR_GAP)
#define BAR_AREA_H 36

typedef struct {
  float values[MAX_BARS];
  int count;
  int head;
  int sensor_idx;
} history_t;

typedef struct {
  al_info_t info;
  const char *unit;
  const char *fmt;
} sensor_t;

static sensor_t sensors[] = {
    {AL_INFO_SENSOR_CO2, "ppm", "%.0f"},
    {AL_INFO_SENSOR_TEMPERATURE, "°C", "%.1f"},
    {AL_INFO_SENSOR_HUMIDITY, "%", "%.0f"},
    {AL_INFO_SENSOR_VOC, "VOC", "%.0f"},
    {AL_INFO_SENSOR_NOX, "NOx", "%.0f"},
    {AL_INFO_SENSOR_PRESSURE, "hPa", "%.0f"},
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
  int sidx = find_sensor("sensor", "co2");
  sensor_t *s = &sensors[sidx];

  // load history, reset if sensor changed
  history_t hist = {0};
  al_data_get("hist", &hist, sizeof(hist));
  if (hist.sensor_idx != sidx) {
    hist = (history_t){0};
    hist.sensor_idx = sidx;
  }

  // read current value and append to circular buffer
  float val = al_info(s->info);
  hist.values[hist.head] = val;
  hist.head = (hist.head + 1) % MAX_BARS;
  if (hist.count < MAX_BARS) hist.count++;

  // save history
  al_data_set("hist", &hist, sizeof(hist));

  // find min/max for bar normalization
  float min_val = hist.values[0], max_val = hist.values[0];
  for (int i = 1; i < hist.count; i++) {
    if (hist.values[i] < min_val) min_val = hist.values[i];
    if (hist.values[i] > max_val) max_val = hist.values[i];
  }
  float range = max_val - min_val;
  if (range < 1.0f) range = 1.0f;

  // draw
  al_clear(0);

  // primary value centered in upper area
  char num[32];
  snprintf(num, sizeof(num), s->fmt, val);
  char display[48];
  snprintf(display, sizeof(display), "%s %s", num, s->unit);
  al_write(AL_W / 2, (AL_H - BAR_AREA_H - 24) / 2, 0, 24, 1, display, AL_WRITE_ALIGN_CENTER);

  // bar chart at bottom
  int start = (hist.count < MAX_BARS) ? 0 : hist.head;
  for (int i = 0; i < hist.count; i++) {
    int idx = (start + i) % MAX_BARS;
    float v = hist.values[idx];
    int bar_h = (int)((v - min_val) / range * (BAR_AREA_H - 2)) + 2;
    int x = i * BAR_STEP;
    al_rect(x, AL_H - bar_h, BAR_W, bar_h, 1, 1);
  }

  return 0;
}
