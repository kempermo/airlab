#ifndef AL_TOUCH_H
#define AL_TOUCH_H

/**
 * The touch hook.
 *
 * @param position The touch position from -3 to 3 or NaN if released.
 */
typedef void (*al_touch_hook_t)(float position);

/**
 * Configure the touch sensor.
 *
 * @param hook The touch hook.
 */
void al_touch_config(al_touch_hook_t hook);

#endif  // AL_TOUCH_H
