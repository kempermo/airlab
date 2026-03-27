#include "../../al.h"
#include "fnt_custom.h"
#include "fnt_comic.h"

// unified glyph type — identical layout to both fnt_glyph_t and comic_glyph_t
typedef struct {
  char ch;
  int w, h;
  const uint8_t *img;
  const uint8_t *mask;
} glyph_t;

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

static const glyph_t *find_glyph(const glyph_t *table, int count, char c) {
  for (int i = 0; i < count; i++) {
    if (table[i].ch == c) return &table[i];
  }
  return NULL;
}

static int measure(const glyph_t *table, int count, const char *str) {
  int w = 0;
  for (const char *p = str; *p; p++) {
    const glyph_t *g = find_glyph(table, count, *p);
    if (g) w += g->w + 2;
  }
  return w > 0 ? w - 2 : 0;
}

static void draw_str(const glyph_t *table, int count, int x, int y, const char *str) {
  for (const char *p = str; *p; p++) {
    const glyph_t *g = find_glyph(table, count, *p);
    if (g) {
      al_draw(x, y, g->w, g->h, 1, 0, g->img, g->mask);
      x += g->w + 2;
    }
  }
}

int main() {
  int sidx = find_sensor("sensor", "co2");
  sensor_t *s = &sensors[sidx];

  // select font
  char font_cfg[16] = {0};
  al_config_get_s("font", font_cfg, sizeof(font_cfg));
  int use_comic = strcmp(font_cfg, "comic") == 0;

  const glyph_t *table = use_comic
      ? (const glyph_t *)comic_glyphs
      : (const glyph_t *)fnt_glyphs;
  int count = use_comic ? COMIC_GLYPH_COUNT : FNT_GLYPH_COUNT;

  float val = al_info(s->info);
  char buf[16];
  snprintf(buf, sizeof(buf), s->fmt, val);

  int str_w = measure(table, count, buf);

  int glyph_h = 0;
  for (const char *p = buf; *p; p++) {
    const glyph_t *g = find_glyph(table, count, *p);
    if (g && g->h > glyph_h) glyph_h = g->h;
  }

  int total_h = glyph_h + 4 + 16;
  int y_start = (AL_H - total_h) / 2 + 6;

  al_clear(0);

  draw_str(table, count, (AL_W - str_w) / 2, y_start, buf);

  al_write(AL_W / 2, y_start + glyph_h + 14, 0, 16, 1, s->unit, AL_WRITE_ALIGN_CENTER);

  return 0;
}
