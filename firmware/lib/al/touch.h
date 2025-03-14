#ifndef AL_TOUCH_H
#define AL_TOUCH_H

#include <stdint.h>

/**
 * The touch event.
 */
typedef struct {
  uint8_t touches;
  float position;
  float delta;
} al_touch_event_t;

/**
 * The touch hook.
 */
typedef void (*al_touch_hook_t)(al_touch_event_t);

/**
 * Configure the touch sensor.
 *
 * @param hook The touch hook.
 */
void al_touch_config(al_touch_hook_t hook);

#endif  // AL_TOUCH_H
