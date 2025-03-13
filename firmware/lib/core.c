#include "buzzer.h"
#include "led.h"
#include "accel.h"

void al_init() {
  // initialize
  al_buzzer_init();
  al_led_init();
  al_accel_init();
}
