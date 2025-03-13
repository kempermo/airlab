#include "buzzer.h"
#include "led.h"

void al_init() {
  // initialize
  al_buzzer_init();
  al_led_init();
}
