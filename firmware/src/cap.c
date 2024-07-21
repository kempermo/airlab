#include <naos.h>
#include <naos/sys.h>
#include <driver/i2c.h>

#define CAP_ADDR 0x37
#define CAP_DEBUG true

// TODO: We need to adjust the current stored configuration:
//  https://www.infineon.com/dgdl/Infineon-CY8CMBR3xxx_CapSense_Express_Controllers_Registers_TRM-AdditionalTechnicalInformation-v06_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f90b2ad7da7&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-technical_reference_manual&redirId=TRM34

static void cap_read(uint8_t reg, uint8_t *buf, size_t len) {
  // read data
  ESP_ERROR_CHECK(i2c_master_write_read_device(I2C_NUM_0, CAP_ADDR, &reg, 1, buf, len, 1000));
}

static uint8_t cap_read8(uint8_t reg) {
  uint8_t value;
  cap_read(reg, &value, 1);
  return value;
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

static void cap_exec(uint8_t cmd) {
  // execute command
  if (CAP_DEBUG) {
    naos_log("cap: exec cmd=%02x", cmd);
  }
  cap_write8(0x86, cmd);

  // wait
  while (cap_read8(0x86) & 0x01) {
    if (CAP_DEBUG) {
      naos_log("cap: wait...");
    }
    naos_delay(1);
  }
  if (CAP_DEBUG) {
    naos_log("cap: done");
  }
}

void cap_check() {
  // read touches
  uint8_t touches[2];
  cap_read(0xAA, touches, 2);
  naos_log("cap: touches=%02x/%02x", touches[0], touches[1]);
}

void cap_init() {
  naos_delay(2000);

  // reset device
  cap_write8(0x86, 0xFF);

  // configure CS7 as shield (SPO1)
  cap_write8(0x4c, 0b00100000);

  // enable shield
  // cap_write8(0x4f, 0b00000001);

  // enable sensors
  cap_write8(0x00, 0b01111111);

  // calculate checksum
  cap_exec(3);

  // store configuration
  cap_exec(2);

  // reset device
  cap_write8(0x86, 0xFF);

  // run check
  naos_repeat("cap", 100, cap_check);
}
