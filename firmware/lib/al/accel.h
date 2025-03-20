#ifndef AL_ACCEL_H
#define AL_ACCEL_H

#include <stdbool.h>
#include <stdint.h>

/**
 * The accelerometer state.
 *
 * @param front Whether the device is front facing.
 * @param rotation The rotation of the device.
 * @param locked Whether the device is gimbal locked.
 */
typedef struct {
  bool front;
  uint16_t rotation;
  bool locked;
} al_accel_state_t;

/**
 * The accelerometer hook.
 */
typedef void (*al_accel_hook_t)(al_accel_state_t);

/**
 * Configure the acceleration sensor.
 *
 * @param hook The accel hook.
 */
void al_accel_config(al_accel_hook_t hook);

/**
 * Returns the cached accelerometer state.
 */
al_accel_state_t al_accel_get();

#endif  // AL_ACCEL_H
