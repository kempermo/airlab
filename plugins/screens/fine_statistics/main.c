#include "../../al.h"

#define NUM_SENSORS   6
#define SPARK_W     240
#define SPARK_PAD     3
#define ROW_H       (AL_H / NUM_SENSORS)
#define SPAN_MS     (5 * 60 * 1000)  // 60 minutes

typedef struct {
  al_info_t        info;
  al_query_field_t field;
  const char      *unit;
  const char      *fmt;
} sensor_t;

static sensor_t sensors[NUM_SENSORS] = {
    {AL_INFO_SENSOR_CO2,         AL_QUERY_CO2, "ppm", "%.0f"},
    {AL_INFO_SENSOR_TEMPERATURE, AL_QUERY_TMP, "\xc2\xb0""C",  "%.1f"},
    {AL_INFO_SENSOR_HUMIDITY,    AL_QUERY_HUM, "%RH", "%.1f"},
    {AL_INFO_SENSOR_VOC,         AL_QUERY_VOC, "VOC", "%.0f"},
    {AL_INFO_SENSOR_NOX,         AL_QUERY_NOX, "NOX", "%.0f"},
    {AL_INFO_SENSOR_PRESSURE,    AL_QUERY_PRS, "hPa", "%.0f"},
};

int main() {
  // determine time range (shared across all sensors)
  int store_stop = (int)al_info(AL_INFO_STORE_STOP);
  int resolution = SPAN_MS / SPARK_W;
  int first = store_stop - SPAN_MS;
  int count = SPARK_W;
  if (first < 0) {
    first = 0;
    count = store_stop / resolution;
    if (count < 1 && store_stop > 0) count = 1;
    if (count > SPARK_W) count = SPARK_W;
  }

  al_clear(0);

  float values[SPARK_W];

  for (int si = 0; si < NUM_SENSORS; si++) {
    sensor_t *s = &sensors[si];
    int row_y    = si * ROW_H;
    int spark_top = row_y + SPARK_PAD;
    int spark_bot = row_y + ROW_H - SPARK_PAD;
    int spark_h   = spark_bot - spark_top;

    // query history for this sensor
    int n = (resolution > 0 && count > 0) ? al_query(s->field, values, count, first, resolution) : 0;

    // min/max for dynamic normalization
    float s_min = 0, s_max = 0;
    if (n > 0) {
      s_min = values[0];
      s_max = values[0];
      for (int j = 1; j < n; j++) {
        if (values[j] < s_min) s_min = values[j];
        if (values[j] > s_max) s_max = values[j];
      }
    }
    float range = s_max - s_min;
    if (range < 1.0f) {
      s_min -= 0.5f;
      range = 1.0f;
    }

    // draw sparkline, right-aligned
    int offset = SPARK_W - n;
    for (int j = 0; j < n; j++) {
      float norm = (values[j] - s_min) / range;
      int x = offset + j;
      int y = spark_bot - (int)(norm * spark_h + 0.5f);
      al_rect(x - 1, y - 1, 3, 3, 1, 0);
    }

    /*
    // row separator right of sparkline
    if (si < NUM_SENSORS - 1) {
      al_line(SPARK_W, row_y + ROW_H, AL_W - 1, row_y + ROW_H, 1, 1);
    }
    */
    // current value + unit, right-aligned
    float val = al_info(s->info);
    char val_str[16], buf[32];
    snprintf(val_str, sizeof(val_str), s->fmt, val);
    snprintf(buf, sizeof(buf), "%s %s", val_str, s->unit);
    al_write(AL_W - 50, row_y + (ROW_H - 8) / 2, 0, 8, 1, buf, 0);
  }

  return 0;
}
