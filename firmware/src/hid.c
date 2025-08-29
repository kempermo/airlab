#include <naos.h>
#include <naos/sys.h>
#include <tinyusb.h>
#include <tinyusb_default_config.h>
#include <class/hid/hid_device.h>

#include <al/core.h>

#include "sig.h"

#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
};

const char *hid_string_descriptor[5] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},     // 0: is supported language is English (0x0409)
    "TinyUSB",                // 1: Manufacturer
    "TinyUSB Device",         // 2: Product
    "123456",                 // 3: Serials, should use chip ID
    "Example HID interface",  // 4: HID
};

static const uint8_t hid_configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16, 10),
};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
  (void)instance;

  return hid_report_descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}

static void hid_event_handler(tinyusb_event_t *event, void *arg) {
  // log event
  naos_log("hid: event %d", event->id);
}

void hid_run() {
  // enable tinyusb stack
  naos_log("hid: enable");
  tinyusb_config_t usb_cfg = TINYUSB_DEFAULT_CONFIG(hid_event_handler);
  usb_cfg.descriptor.string = hid_string_descriptor;
  usb_cfg.descriptor.string_count = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]);
  usb_cfg.descriptor.full_speed_config = hid_configuration_descriptor;
  usb_cfg.phy.self_powered = true;
  usb_cfg.phy.vbus_monitor_io = AL_USB_MON;
  ESP_ERROR_CHECK(tinyusb_driver_install(&usb_cfg));

  // await mount
  while (!tud_mounted()) {
    naos_delay(100);
  }

  // perform loop
  while (tud_mounted()) {
    // await event
    sig_event_t event = sig_await(SIG_KEYS, 1000);
    if (event.type == SIG_TIMEOUT) {
      continue;
    }

    // reports keys
    if (event.type & SIG_KEYS) {
      // determine keycode
      uint8_t keycode[6] = {0};
      if (event.type == SIG_ENTER) {
        keycode[0] = HID_KEY_A;
      } else if (event.type == SIG_ESCAPE) {
        keycode[0] = HID_KEY_B;
      } else if (event.type == SIG_UP) {
        keycode[0] = HID_KEY_ARROW_UP;
      } else if (event.type == SIG_DOWN) {
        keycode[0] = HID_KEY_ARROW_DOWN;
      } else if (event.type == SIG_LEFT) {
        keycode[0] = HID_KEY_ARROW_LEFT;
      } else if (event.type == SIG_RIGHT) {
        keycode[0] = HID_KEY_ARROW_RIGHT;
      }

      // send keycode
      tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, keycode);
      naos_delay(10);
      tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL);
    }
  }

  // log
  naos_log("hid: disable");
  ESP_ERROR_CHECK(tinyusb_driver_uninstall());
}
