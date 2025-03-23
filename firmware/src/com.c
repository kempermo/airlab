#include <naos.h>
#include <naos/fs.h>
#include <naos/ble.h>
#include <naos/wifi.h>
#include <naos/mqtt.h>
#include <naos/sys.h>

#include <al/sensor.h>

#include "dat.h"

static bool com_mqtt_ha = false;

static void com_task() {
  // wait some time
  naos_delay(5000);

  // run network
  naos_fs_install((naos_fs_config_t){.root = DAT_ROOT});
  naos_ble_init((naos_ble_config_t){});
  naos_wifi_init();
  naos_mqtt_init(1);

  for (;;) {
    // await sample
    al_sample_t sample = al_sensor_next();

    // check connection
    if (naos_status() != NAOS_NETWORKED) {
      continue;
    }

    // publish sample
    if (com_mqtt_ha) {
      naos_publish_d("airlab/co2", al_sample_read(sample, AL_SAMPLE_CO2), 0, false, NAOS_GLOBAL);
      naos_publish_d("airlab/tmp", al_sample_read(sample, AL_SAMPLE_TMP), 0, false, NAOS_GLOBAL);
      naos_publish_d("airlab/hum", al_sample_read(sample, AL_SAMPLE_HUM), 0, false, NAOS_GLOBAL);
      naos_publish_d("airlab/voc", al_sample_read(sample, AL_SAMPLE_VOC), 0, false, NAOS_GLOBAL);
      naos_publish_d("airlab/nox", al_sample_read(sample, AL_SAMPLE_NOX), 0, false, NAOS_GLOBAL);
      naos_publish_d("airlab/prs", al_sample_read(sample, AL_SAMPLE_PRS), 0, false, NAOS_GLOBAL);
    } else {
      naos_publish_d("co2", al_sample_read(sample, AL_SAMPLE_CO2), 0, false, NAOS_LOCAL);
      naos_publish_d("tmp", al_sample_read(sample, AL_SAMPLE_TMP), 0, false, NAOS_LOCAL);
      naos_publish_d("hum", al_sample_read(sample, AL_SAMPLE_HUM), 0, false, NAOS_LOCAL);
      naos_publish_d("voc", al_sample_read(sample, AL_SAMPLE_VOC), 0, false, NAOS_LOCAL);
      naos_publish_d("nox", al_sample_read(sample, AL_SAMPLE_NOX), 0, false, NAOS_LOCAL);
      naos_publish_d("prs", al_sample_read(sample, AL_SAMPLE_PRS), 0, false, NAOS_LOCAL);
    }
  }
}

static naos_param_t com_params[] = {
    {.name = "mqtt-ha", .type = NAOS_BOOL, .sync_b = &com_mqtt_ha},
};

void com_init() {
  // register params
  for (size_t i = 0; i < sizeof(com_params) / sizeof(naos_param_t); i++) {
    naos_register(&com_params[i]);
  }

  // run task
  naos_run("com", 4096, 1, com_task);
}

void com_online() {
  // check if home assistant is enabled
  if (!com_mqtt_ha) {
    return;
  }

  // prepare discovery messages
  const char* co2 =
      "{ \"name\": \"Air Lab CO2\", \"state_topic\": \"airlab/co2\", \"unit_of_measurement\": \"ppm\", "
      "\"device_class\": \"carbon_dioxide\", \"state_class\": \"measurement\", \"unique_id\": \"al_co2\", \"device\": "
      "{ \"identifiers\": [\"al_01\"], \"name\": \"Air Lab\", \"manufacturer\": \"Networked Artifacts\", \"model\": "
      "\"R3-2025\" } }";
  const char* tmp =
      "{ \"name\": \"Air Lab TMP\", \"state_topic\": \"airlab/tmp\", \"unit_of_measurement\": \"°C\", "
      "\"device_class\": \"temperature\", \"state_class\": \"measurement\", \"unique_id\": \"al_tmp\", \"device\": { "
      "\"identifiers\": [\"al_01\"], \"name\": \"Air Lab\", \"manufacturer\": \"Networked Artifacts\", \"model\": "
      "\"R3-2025\" } }";
  const char* hum =
      "{ \"name\": \"Air Lab HUM\", \"state_topic\": \"airlab/hum\", \"unit_of_measurement\": \"%\", \"device_class\": "
      "\"humidity\", \"state_class\": \"measurement\", \"unique_id\": \"al_hum\", \"device\": { \"identifiers\": "
      "[\"al_01\"], \"name\": \"Air Lab\", \"manufacturer\": \"Networked Artifacts\", \"model\": \"R3-2025\" } }";
  const char* voc =
      "{ \"name\": \"Air Lab VOC\", \"state_topic\": \"airlab/voc\", \"unit_of_measurement\": \"\", \"device_class\": "
      "\"aqi\", \"state_class\": \"measurement\", \"unique_id\": \"al_voc\", \"device\": { \"identifiers\": "
      "[\"al_01\"], \"name\": \"Air Lab\", \"manufacturer\": \"Networked Artifacts\", \"model\": \"R3-2025\" } }";
  const char* nox =
      "{ \"name\": \"Air Lab NOX\", \"state_topic\": \"airlab/nox\", \"unit_of_measurement\": \"\", \"device_class\": "
      "\"aqi\", \"state_class\": \"measurement\", \"unique_id\": \"al_nox\", \"device\": { \"identifiers\": "
      "[\"al_01\"], \"name\": \"Air Lab\", \"manufacturer\": \"Networked Artifacts\", \"model\": \"R3-2025\" } }";
  const char* prs =
      "{ \"name\": \"Air Lab PRS\", \"state_topic\": \"airlab/prs\", \"unit_of_measurement\": \"hPa\", "
      "\"device_class\": \"atmospheric_pressure\", \"state_class\": \"measurement\", \"unique_id\": \"al_prs\", "
      "\"device\": { \"identifiers\": [\"al_01\"], \"name\": \"Air Lab\", \"manufacturer\": \"Networked Artifacts\", "
      "\"model\": \"R3-2025\" } }";

  // publish discovery messages
  naos_publish_s("homeassistant/sensor/al_co2/config", co2, 0, true, NAOS_GLOBAL);
  naos_publish_s("homeassistant/sensor/al_tmp/config", tmp, 0, true, NAOS_GLOBAL);
  naos_publish_s("homeassistant/sensor/al_hum/config", hum, 0, true, NAOS_GLOBAL);
  naos_publish_s("homeassistant/sensor/al_voc/config", voc, 0, true, NAOS_GLOBAL);
  naos_publish_s("homeassistant/sensor/al_nox/config", nox, 0, true, NAOS_GLOBAL);
  naos_publish_s("homeassistant/sensor/al_prs/config", prs, 0, true, NAOS_GLOBAL);
}
