#ifndef AL_CORE_H
#define AL_CORE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * The available triggers that can wake the device.
 */
typedef enum {
  AL_NONE,
  AL_TIMEOUT,
  AL_UNLOCK,
} al_trigger_t;

/**
 * Initializes the Air Lab library.
 */
void al_init();

/**
 * Puts the device into light/deep sleep for the specified timeout or indefinitely.
 *
 * @param deep Whether to enter deep sleep.
 * @param timeout The timeout in milliseconds or 0 for indefinite sleep.
 */
void al_sleep(bool deep, uint64_t timeout);

/**
 * Returns the trigger that woke the device.
 *
 * @return The trigger that woke the device.
 */
al_trigger_t al_trigger();

#endif  // AL_CORE_H
