#include <stdbool.h>

#include "../al.h"

int main() {
  // configure GPIO
  al_gpio(AL_GPIO_CONFIG, AL_GPIO_A | AL_GPIO_ANALOG_INPUT, 0);

  for (;;) {
    // read value
    int val = al_gpio(AL_GPIO_READ_ANALOG, AL_GPIO_A, 0);

    // clear screen
    al_clear(0);

    // show value
    char buf[32];
    snprintf(buf, sizeof(buf), "Value: %d", val);
    al_write(AL_W / 2, AL_H / 2 - 8, 0, 16, 1, buf, AL_WRITE_ALIGN_CENTER);

    // await event
    al_yield_result_t res = al_yield(100, 0);

    // stop on escape
    if (res == AL_YIELD_ESCAPE) {
      break;
    }
  }

  return 0;
}
