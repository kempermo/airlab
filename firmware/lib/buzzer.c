#include <naos.h>
#include <naos/sys.h>
#include <driver/ledc.h>
#include <rom/ets_sys.h>

#define BUZZER_DEBUG false

// Component: PKMCS0909E

static naos_mutex_t al_buzzer_mutex;
static bool al_buzzer_beeping = false;

static void al_buzzer_tone(int hz, int us) {
  // start beep
  ESP_ERROR_CHECK(ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, hz));
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512));
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 512));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

  // wait some time
  ets_delay_us(us);

  // stop channels
  ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
  ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 1));
}

void al_buzzer_init() {
  // check state
  al_buzzer_mutex = naos_mutex();

  // setup timer
  ledc_timer_config_t ledc_timer = {
      .freq_hz = 440,
      .duty_resolution = LEDC_TIMER_10_BIT,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .timer_num = LEDC_TIMER_0,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // setup channels
  ledc_channel_config_t ch1 = {
      .channel = LEDC_CHANNEL_0,
      .duty = 0,
      .gpio_num = GPIO_NUM_5,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .hpoint = 0,
      .timer_sel = LEDC_TIMER_0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ch1));
  ledc_channel_config_t ch2 = {
      .channel = LEDC_CHANNEL_1,
      .duty = 0,
      .gpio_num = GPIO_NUM_46,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .hpoint = 0,
      .timer_sel = LEDC_TIMER_0,
      .flags.output_invert = true,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ch2));

  // stop channels
  ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
  ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 1));
}

void al_buzzer_click() {
  // check beeping
  naos_lock(al_buzzer_mutex);
  bool beeping = al_buzzer_beeping;
  naos_unlock(al_buzzer_mutex);
  if (beeping) {
    return;
  }

  // make tone
  al_buzzer_tone(8000, 125);
}

void al_buzzer_beep(int hz, int ms) {
  // check arguments
  if (hz == 0 || ms == 0) {
    return;
  }

  // check flag
  naos_lock(al_buzzer_mutex);
  bool was_beeping = al_buzzer_beeping;
  al_buzzer_beeping = true;
  naos_unlock(al_buzzer_mutex);
  if (was_beeping) {
    return;
  }

  // make tone
  al_buzzer_tone(hz, ms * 1000);
  if (BUZZER_DEBUG) {
    naos_log("al-bzr: done");
  }

  // clear flag
  naos_lock(al_buzzer_mutex);
  al_buzzer_beeping = false;
  naos_unlock(al_buzzer_mutex);
}
