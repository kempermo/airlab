#include <esp_err.h>

#include <al/led.h>

#include "internal.h"

#define AL_LED_ADDR 0x30
#define AL_LED_LIMIT(val) (val < 0 ? 0 : val > 1 ? 1 : val)

static void al_led_write(uint8_t reg, uint8_t val, bool may_fail) {
  // write data
  uint8_t data[2] = {reg, val};
  esp_err_t err = al_i2c_transfer(AL_LED_ADDR, data, 2, NULL, 0, 1000);
  if (!may_fail || err != ESP_FAIL) {
    // TODO: LED writes may fail after waking from light sleep.
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
  }
}

static void al_led_reset() {
  // reset chip
  al_led_write(0x00, 0b00000111, true);

  // disable auto blink
  al_led_write(0x09, 0b00000110, false);

  // turn LEDs off
  al_led_set(0, 0, 0);
}

void al_led_init(bool reset) {
  // perform reset
  if (reset) {
    al_led_reset();
  }
}

void al_led_set(float r, float g, float b) {
  // scale values to 0-17mA
  r *= 0.7f;
  g *= 0.7f;
  b *= 0.7f;

  // set LEDs on/off
  uint8_t state = (b > 0) | (g > 0) << 2 | (r > 0) << 4;
  al_led_write(0x04, 0b01000000 | state, false);

  // set color
  al_led_write(0x06, (uint8_t)(AL_LED_LIMIT(b) * 191), false);
  al_led_write(0x07, (uint8_t)(AL_LED_LIMIT(g) * 191), false);
  al_led_write(0x08, (uint8_t)(AL_LED_LIMIT(r) * 191), false);
}

void al_led_flash(float r, float g, float b) {
  // configure flash period
  al_led_write(0x01, 38, false);  // ~5s (0.256 + v * 0.128)

  // configure ON percentage
  al_led_write(0x02, 12, false);  // 12% = 0.6s
  al_led_write(0x03, 12, false);  // 12% = 0.6s

  // configure rise/fall times
  al_led_write(0x05, 0b00100010, false);  // 256/256ms

  // set LEDs on/off
  uint8_t state = (b > 0) << 1 | (g > 0) << 3 | (r > 0) << 5;
  al_led_write(0x04, 0b01000000 | state, false);

  // set color
  al_led_write(0x06, (uint8_t)(AL_LED_LIMIT(b) * 191), false);
  al_led_write(0x07, (uint8_t)(AL_LED_LIMIT(g) * 191), false);
  al_led_write(0x08, (uint8_t)(AL_LED_LIMIT(r) * 191), false);
}
