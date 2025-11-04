#include <stdbool.h>

#include "../al.h"

#define MIN_DUTY 160
#define MAX_DUTY 420

int main() {
  // configure GPIO
  al_gpio(AL_GPIO_CONFIG, AL_GPIO_A | AL_GPIO_PWM_OUTPUT, 50);

  // clear screen
  al_clear(0);

  // prepare duty
  int duty = MIN_DUTY + ((MAX_DUTY - MIN_DUTY) / 2);

  for (;;) {
    // set duty
    al_gpio(AL_GPIO_WRITE_PWM, AL_GPIO_A, duty);

    // log duty
    al_logf("duty=%d", duty);

    // await event
    al_yield_result_t res = al_yield(0, 0);

    // stop on escape
    if (res == AL_YIELD_ESCAPE) {
      break;
    }

    // adjust duty
    if (res == AL_YIELD_UP) {
      duty += 10;
      if (duty > MAX_DUTY) {
        duty = MAX_DUTY;
      }
    } else if (res == AL_YIELD_DOWN) {
      duty -= 10;
      if (duty < MIN_DUTY) {
        duty = MIN_DUTY;
      }
    }
  }

  return 0;
}
