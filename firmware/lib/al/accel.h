#ifndef AL_ACCEL_H
#define AL_ACCEL_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool front;
  uint16_t rot;
  bool lock;
} al_accel_state_t;

al_accel_state_t al_accel_get();

#endif  // AL_ACCEL_H
