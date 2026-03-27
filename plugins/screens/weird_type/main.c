#include "../../al.h"
#include "fnt_custom.h"

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

static const fnt_glyph_t *find_glyph(char c) {
  for (int i = 0; i < FNT_GLYPH_COUNT; i++) {
    if (fnt_glyphs[i].ch == c) return &fnt_glyphs[i];
  }
  return NULL;
}

// measure total pixel width of a string using custom font
static int measure(const char *str) {
  int w = 0;
  for (const char *p = str; *p; p++) {
    const fnt_glyph_t *g = find_glyph(*p);
    if (g) w += g->w + 2;
  }
  return w > 0 ? w - 2 : 0;
}

// draw string using custom font, baseline-aligned to given y (top of tallest glyph)
static void draw_str(int x, int y, const char *str) {
  for (const char *p = str; *p; p++) {
    const fnt_glyph_t *g = find_glyph(*p);
    if (g) {
      al_draw(x, y, g->w, g->h, 1, 0, g->img, g->mask);
      x += g->w + 2;
    }
  }
}

int main() {
  int sidx = find_sensor("sensor", "co2");
  sensor_t *s = &sensors[sidx];

  float val = al_info(s->info);

  char buf[16];
  snprintf(buf, sizeof(buf), s->fmt, val);

  // measure for centering
  int str_w = measure(buf);

  // find max glyph height for vertical centering
  int glyph_h = 0;
  for (const char *p = buf; *p; p++) {
    const fnt_glyph_t *g = find_glyph(*p);
    if (g && g->h > glyph_h) glyph_h = g->h;
  }

  // vertical layout: value + gap + unit
  int total_h = glyph_h + 4 + 16;
  int y_start = (AL_H - total_h) / 2;

  al_clear(0);

  // draw value with custom font, centered
  draw_str((AL_W - str_w) / 2, y_start, buf);

  // draw unit with built-in font, centered below
  al_write(AL_W / 2, y_start + glyph_h + 4, 0, 16, 1, s->unit, AL_WRITE_ALIGN_CENTER);

  return 0;
}
