#include <naos.h>
#include <naos/sys.h>
#include <driver/rmt_tx.h>

#define BUZZER_DEBUG false

// Component: PKMCS0909E

static naos_mutex_t al_buzzer_mutex;
static bool al_buzzer_beeping = false;
static rmt_channel_handle_t al_buzzer_channel;
static rmt_encoder_handle_t al_buzzer_encoder;

void al_buzzer_init() {
  // check state
  al_buzzer_mutex = naos_mutex();

  // setup buzzer
  rmt_tx_channel_config_t rmt_cfg = {
      .gpio_num = GPIO_NUM_5,
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = 1000 * 1000,  // 1 us
      .mem_block_symbols = 48,
      .trans_queue_depth = 16,
  };
  ESP_ERROR_CHECK(rmt_new_tx_channel(&rmt_cfg, &al_buzzer_channel));
  ESP_ERROR_CHECK(rmt_enable(al_buzzer_channel));

  // setup buzzer encoder
  rmt_copy_encoder_config_t enc_cfg = {};
  ESP_ERROR_CHECK(rmt_new_copy_encoder(&enc_cfg, &al_buzzer_encoder));
}

void al_buzzer_click() {
  // check beeping
  naos_lock(al_buzzer_mutex);
  bool beeping = al_buzzer_beeping;
  naos_unlock(al_buzzer_mutex);
  if (beeping) {
    return;
  }

  // prepare buzz
  rmt_symbol_word_t item = {
      .level0 = 1,
      .duration0 = 125,  // us
      .level1 = 0,
      .duration1 = 1,
  };

  // perform buzz
  rmt_transmit_config_t cfg = {
      .flags.eot_level = 0,
      .flags.queue_nonblocking = 1,
  };
  ESP_ERROR_CHECK(rmt_transmit(al_buzzer_channel, al_buzzer_encoder, &item, sizeof(item), &cfg));
}

void al_buzzer_beep(int hz, int ms, bool wait) {
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

  // calculate parameters
  int period_us = 1000000 / hz;
  int cycles = (ms * 1000) / period_us;
  if (BUZZER_DEBUG) {
    naos_log("al-bzr: beep %dHz for %dms (%d cycles, period %dus)", hz, ms, cycles, period_us);
  }

  // generate waveform
  rmt_symbol_word_t waveform[cycles];
  for (int i = 0; i < cycles; i++) {
    waveform[i] = (rmt_symbol_word_t){
        .level0 = 1,
        .duration0 = period_us / 2,
        .level1 = 0,
        .duration1 = period_us / 2,
    };
  }

  // perform tone
  rmt_transmit_config_t cfg = {
      .flags.eot_level = 0,
  };
  ESP_ERROR_CHECK(rmt_transmit(al_buzzer_channel, al_buzzer_encoder, waveform, sizeof(waveform), &cfg));
  if (wait) {
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(al_buzzer_channel, -1));
  }
  if (BUZZER_DEBUG) {
    naos_log("al-bzr: done");
  }

  // clear flag
  naos_lock(al_buzzer_mutex);
  al_buzzer_beeping = false;
  naos_unlock(al_buzzer_mutex);
}
