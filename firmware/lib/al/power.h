#ifndef AL_POWER_H
#define AL_POWER_H

#include <stdbool.h>

typedef struct {
  float battery;
  bool usb;
  bool fast;
  bool charging;
} al_power_state_t;

al_power_state_t al_power_get();

void al_power_off();

void al_power_ship();

#endif  // AL_POWER_H
