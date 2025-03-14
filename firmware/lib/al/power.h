#ifndef AL_POWER_H
#define AL_POWER_H

#include <stdbool.h>

/**
 * The power state.
 */
typedef struct {
  float battery;
  bool usb;
  bool fast;
  bool charging;
} al_power_state_t;

/**
 * Returns the current power state.
 */
al_power_state_t al_power_get();

/**
 * Turns the device power off.
 */
void al_power_off();

/**
 * Puts the device into ship mode.
 */
void al_power_ship();

#endif  // AL_POWER_H
