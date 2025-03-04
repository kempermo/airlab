#include <driver/i2c.h>

#include "led.h"

#define LED_ADDR 0x30
#define LED_LIMIT(val) (val < 0 ? 0 : val > 1 ? 1 : val)

static void led_write(uint8_t reg, uint8_t val, bool may_fail) {
  // write data
  uint8_t data[2] = {reg, val};
  esp_err_t err = i2c_master_write_to_device(I2C_NUM_0, LED_ADDR, data, 2, 1000);
  if (!may_fail || err != ESP_FAIL) {
    // TODO: LED writes may fail after waking from light sleep.
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
  }
}

void led_init() {
  // reset chip
  led_write(0x00, 0b00000111, true);

  // disable auto blink
  led_write(0x09, 0b00000110, false);

  // turn LEDs off
  led_set(0, 0, 0);
}

void led_set(float r, float g, float b) {
  // scale values to 0-17mA
  r *= 0.7f;
  g *= 0.7f;
  b *= 0.7f;

  // set LEDs on/off
  uint8_t state = (b > 0) | (g > 0) << 2 | (r > 0) << 4;
  led_write(0x04, 0b01000000 | state, false);

  // set color
  led_write(0x06, (uint8_t)(LED_LIMIT(b) * 191), false);
  led_write(0x07, (uint8_t)(LED_LIMIT(g) * 191), false);
  led_write(0x08, (uint8_t)(LED_LIMIT(r) * 191), false);
}

void led_flash(float r, float g, float b) {
  // configure flash period
  led_write(0x01, 38, false);  // ~5s (0.256 + v * 0.128)

  // configure ON percentage
  led_write(0x02, 12, false);  // 12% = 0.6s
  led_write(0x03, 12, false);  // 12% = 0.6s

  // configure rise/fall times
  led_write(0x05, 0b00100010, false);  // 256/256ms

  // set LEDs on/off
  uint8_t state = (b > 0) << 1 | (g > 0) << 3 | (r > 0) << 5;
  led_write(0x04, 0b01000000 | state, false);

  // set color
  led_write(0x06, (uint8_t)(LED_LIMIT(b) * 191), false);
  led_write(0x07, (uint8_t)(LED_LIMIT(g) * 191), false);
  led_write(0x08, (uint8_t)(LED_LIMIT(r) * 191), false);
}
