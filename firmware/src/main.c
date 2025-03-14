#include <naos.h>
#include <naos/cpu.h>
#include <naos/sys.h>

#include <al/core.h>
#include <al/clock.h>
#include <al/touch.h>
#include <al/buzzer.h>

#include "dev.h"
#include "sig.h"
#include "btn.h"
#include "gfx.h"
#include "sns.h"
#include "dat.h"
#include "rec.h"
#include "scr.h"
#include "sys.h"

static naos_mutex_t hmi_mutex;
static float hmi_touch_delta;

static void hmi_touch_hook(al_touch_event_t event) {
  // buzz
  al_buzzer_tick();

  // stop if not touched
  if (event.touches == 0) {
    return;
  }

  // update delta
  naos_lock(hmi_mutex);
  hmi_touch_delta += event.delta;
  naos_unlock(hmi_mutex);

  // dispatch event
  sig_dispatch((sig_event_t){
      .type = SIG_TOUCH,
      .touch = event.position,
  });
}

static void hmi_touch_monitor() {
  // capture delta
  naos_lock(hmi_mutex);
  float delta = hmi_touch_delta;
  hmi_touch_delta = 0;
  naos_unlock(hmi_mutex);

  // stop, if zero
  if (delta == 0) {
    return;
  }

  // dispatch event
  sig_dispatch((sig_event_t){
      .type = SIG_SCROLL,
      .touch = delta,
  });
}

static void setup() {
  // log
  naos_log("setup");

  // init core
  al_init();

  // initialize
  sig_init();
  btn_init();
  gfx_init();
  sns_init();
  dat_init();
  rec_init();

  // setup HMI
  hmi_mutex = naos_mutex();
  al_touch_config(hmi_touch_hook);
  naos_repeat("hmi", 300, hmi_touch_monitor);

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
