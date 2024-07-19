#include <naos.h>
#include <driver/i2c.h>

#define ACC_ADDR 0x18

static void acc_read(uint8_t reg, uint8_t *buf, size_t len) {
  // read data
  ESP_ERROR_CHECK(i2c_master_write_read_device(I2C_NUM_0, ACC_ADDR, &reg, 1, buf, len, 1000));
}

void acc_init() {
  // read status
  uint8_t status;
  acc_read(0x00, &status, 1);
  naos_log("acc: status=%02x", status);

  // read data
  uint8_t data[6];
  acc_read(0x04, data, 6);
  naos_log("acc: data=%02x%02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4], data[5]);
}
