#include "../../al.h"

#define NUM_SENSORS 6
#define HISTORY 110
#define SPARK_W 110
#define SPARK_PAD 3
#define ROW_H (AL_H / NUM_SENSORS)

typedef struct {
  al_info_t info;
  const char *unit;
  const char *fmt;
} sensor_t;

static sensor_t sensors[NUM_SENSORS] = {
    {AL_INFO_SENSOR_CO2,         "ppm",  "%.0f"},
    {AL_INFO_SENSOR_TEMPERATURE, "\xc2\xb0""C",  "%.1f"},
    {AL_INFO_SENSOR_HUMIDITY,    "%RH",  "%.1f"},
    {AL_INFO_SENSOR_VOC,         "VOC",  "%.0f"},
    {AL_INFO_SENSOR_NOX,         "NOX",  "%.0f"},
    {AL_INFO_SENSOR_PRESSURE,    "hPa",  "%.0f"},
};

typedef struct {
  float values[NUM_SENSORS][HISTORY];
  int count;
  int head;
} history_t;

int main() {
  // load history
  history_t hist = {0};
  al_data_get("hist", &hist, sizeof(hist));

  // read all sensors and append
  float current[NUM_SENSORS];
  for (int i = 0; i < NUM_SENSORS; i++) {
    current[i] = al_info(sensors[i].info);
    hist.values[i][hist.head] = current[i];
  }
  hist.head = (hist.head + 1) % HISTORY;
  if (hist.count < HISTORY) hist.count++;

  // save history
  al_data_set("hist", &hist, sizeof(hist));

  al_clear(0);

  int start = (hist.count < HISTORY) ? 0 : hist.head;

  for (int si = 0; si < NUM_SENSORS; si++) {
    int row_y = si * ROW_H;
    int spark_top = row_y + SPARK_PAD;
    int spark_bot = row_y + ROW_H - SPARK_PAD;
    int spark_h = spark_bot - spark_top;

    // find min/max for dynamic normalization
    float s_min = hist.values[si][0];
    float s_max = hist.values[si][0];
    for (int j = 0; j < hist.count; j++) {
      int idx = (start + j) % HISTORY;
      float v = hist.values[si][idx];
      if (v < s_min) s_min = v;
      if (v > s_max) s_max = v;
    }
    float range = s_max - s_min;
    if (range < 1.0f) {
      s_min -= 0.5f;
      range = 1.0f;
    }

    // draw sparkline
    int prev_x = -1, prev_y = -1;
    for (int j = 0; j < hist.count; j++) {
      int idx = (start + j) % HISTORY;
      float v = hist.values[si][idx];
      float norm = (v - s_min) / range;
      int x = j;
      int y = spark_bot - (int)(norm * spark_h + 0.5f);
      if (prev_x >= 0) {
        al_line(prev_x, prev_y, x, y, 1, 1);
      }
      prev_x = x;
      prev_y = y;
    }

    // row separator (right of sparkline area)
    if (si < NUM_SENSORS - 1) {
      al_line(SPARK_W, row_y + ROW_H, AL_W - 1, row_y + ROW_H, 1, 1);
    }

    // value + unit text, right-aligned
    char val_str[16];
    snprintf(val_str, sizeof(val_str), sensors[si].fmt, current[si]);
    char buf[32];
    snprintf(buf, sizeof(buf), "%s %s", val_str, sensors[si].unit);
    al_write(AL_W - 2, row_y + (ROW_H - 16) / 2, 0, 16, 1, buf, AL_WRITE_ALIGN_RIGHT);
  }

  return 0;
}
