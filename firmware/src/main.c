#include <naos.h>
#include <naos/cpu.h>

#include <al/core.h>
#include <al/clock.h>
#include <al/sensor.h>

#include "dev.h"
#include "sig.h"
#include "hmi.h"
#include "gfx.h"
#include "dat.h"
#include "rec.h"
#include "scr.h"
#include "sys.h"

static void sensor_hook(al_sensor_state_t) {
  // dispatch event
  sig_dispatch((sig_event_t){
      .type = SIG_SENSOR,
  });
}

static void setup() {
  // init core
  al_init();

  // print trigger
  naos_log("main: trigger=%d", al_trigger());

  // initialize
  sig_init();
  hmi_init();
  gfx_init();
  dat_init();
  rec_init();

  // wire sensor
  al_sensor_config(sensor_hook);

  // check storage
  dat_info_t info = dat_info();
  naos_log("main: space total=%lu free=%lu usage=%.1f%%", info.total, info.free, info.usage * 100.f);

  // sync time
  al_clock_state_t state = al_clock_get();
  sys_set_date(state.year, state.month, state.day);
  sys_set_time(state.hours, state.minutes, state.seconds);
  naos_log("main: time %02d-%02d-%02d %02d:%02d:%02d", state.year, state.month, state.day, state.hours, state.minutes,
           state.seconds);

  // run screen
  scr_run();
}

static naos_config_t config = {
    .device_type = "airlab",
    .device_version = DEV_VERSION,
    .setup_callback = setup,
};

void app_main() {
  // run naos
  naos_init(&config);
  naos_cpu_init();
  naos_start();
}
