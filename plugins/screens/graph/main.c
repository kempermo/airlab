#include "../../al.h"

#define GRAPH_X 4
#define GRAPH_Y 4
#define GRAPH_W (AL_W - GRAPH_X * 2)
#define GRAPH_H (AL_H - GRAPH_Y - 20)
#define BAR_W 2
#define BAR_S 2
#define BAR_N 72

#define MAX_SPAN 1440

typedef struct {
  const char *name;
  al_query_field_t field;
  const char *unit;
  const char *fmt;
} field_info_t;

static const field_info_t field_table[] = {
    {"co2", AL_QUERY_CO2, "ppm", "%.0f"}, {"tmp", AL_QUERY_TMP, "°C", "%.1f"},  {"hum", AL_QUERY_HUM, "%", "%.1f"},
    {"voc", AL_QUERY_VOC, "VOC", "%.0f"}, {"nox", AL_QUERY_NOX, "NOx", "%.0f"}, {"prs", AL_QUERY_PRS, "hPa", "%.0f"},
};

static const field_info_t *find_field(const char *name) {
  for (int i = 0; i < (int)(sizeof(field_table) / sizeof(field_table[0])); i += 1) {
    if (strcmp(name, field_table[i].name) == 0) return &field_table[i];
  }
  return &field_table[0];
}

int main() {
  // get config
  char field_name[32] = "co2";
  al_config_get_s("field", field_name, sizeof(field_name));
  const field_info_t *fi = find_field(field_name);

  int span = al_config_get_i("span");
  if (span <= 0) span = 60;
  if (span > MAX_SPAN) span = MAX_SPAN;

  // determine time range
  int store_stop = (int)al_info(AL_INFO_STORE_STOP);
  int span_ms = span * 60 * 1000;
  int resolution = span_ms / BAR_N;
  int first = store_stop - span_ms;
  int count = BAR_N;
  if (first < 0) {
    first = 0;
    count = store_stop / resolution;
    if (count < 1 && store_stop > 0) count = 1;
    if (count > BAR_N) count = BAR_N;
  }

  // query samples
  float values[BAR_N] = {0};
  int n = (resolution > 0 && count > 0) ? al_query(fi->field, values, count, first, resolution) : 0;
  al_logf("first=%d stop=%d res=%d count=%d n=%d", first, store_stop, resolution, count, n);

  // find min/max
  float min = 0;
  float max = 0;
  if (n > 0) {
    min = values[0];
    max = values[0];
    for (int i = 1; i < n; i += 1) {
      if (values[i] < min) min = values[i];
      if (values[i] > max) max = values[i];
    }
  }

  // keep original min/max for labels
  float data_min = min;
  float data_max = max;

  // ensure range
  if (max - min < 1.0f) {
    min -= 0.5f;
    max += 0.5f;
  }

  // add margin
  float range = max - min;
  min -= range * 0.1f;
  max += range * 0.1f;
  range = max - min;

  // draw bars
  char buf[32];
  if (n > 0) {
    int offset = (BAR_N - n) * (BAR_W + BAR_S);
    int bottom = GRAPH_Y + GRAPH_H - 1;
    for (int i = 0; i < n; i += 1) {
      int x = GRAPH_X + offset + i * (BAR_W + BAR_S);
      int h = (int)((values[i] - min) / range * (GRAPH_H - 1));
      if (h > 0) {
        al_rect(x, bottom - h, BAR_W, h + 1, 1, 0);
      }
    }
  }

  // draw info line
  int y = AL_H - 14;
  if (n > 0) {
    char num_min[16], num_max[16];
    snprintf(num_min, sizeof(num_min), fi->fmt, data_min);
    snprintf(num_max, sizeof(num_max), fi->fmt, data_max);
    snprintf(buf, sizeof(buf), "min %s  max %s", num_min, num_max);
    al_write(GRAPH_X, y, 0, 12, 1, buf, 0);
    char num[16];
    snprintf(num, sizeof(num), fi->fmt, values[n - 1]);
    snprintf(buf, sizeof(buf), "now %s %s @ %dm", num, fi->unit, span);
    al_write(AL_W - GRAPH_X, y, 0, 12, 1, buf, AL_WRITE_ALIGN_RIGHT);
  } else {
    snprintf(buf, sizeof(buf), "-- %s", fi->unit);
    al_write(AL_W / 2, y, 0, 12, 1, buf, AL_WRITE_ALIGN_CENTER);
  }

  return 0;
}
