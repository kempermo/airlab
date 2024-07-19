#include <naos.h>
#include <naos/sys.h>
#include <driver/i2c.h>

#define CAP_ADDR 0x37

static void cap_read(uint8_t reg, uint8_t *buf, size_t len) {
  // read data
  ESP_ERROR_CHECK(i2c_master_write_read_device(I2C_NUM_0, CAP_ADDR, &reg, 1, buf, len, 1000));
}

static void cap_write(uint8_t reg, const uint8_t *buf, size_t len) {
  static uint8_t data[32];

  // copy data
  data[0] = reg;
  for (size_t i = 0; i < len; i++) {
    data[i + 1] = buf[i];
  }

  // read data
  ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_NUM_0, CAP_ADDR, data, 1 + len, 1000));
}

static void cap_write8(uint8_t reg, uint8_t value) { cap_write(reg, &value, 1); }

void cap_check() {
  // read touches
  uint8_t touches;
  cap_read(0xAA, &touches, 1);
  naos_log("cap: touches=%02x", touches);
}

void cap_init() {
  // reset device
  cap_write8(0x86, 0xFF);

  // enable device
  cap_write8(0x00, 0x01);

  // run check
  naos_repeat("cap", 100, cap_check);
}
