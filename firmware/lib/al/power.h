#ifndef AL_POWER_H
#define AL_POWER_H

typedef struct {
  float battery;
  bool usb;
  bool fast;
  bool charging;
} al_power_state_t;

typedef enum {
  AL_POWER_NONE,
  AL_POWER_TIMEOUT,
  AL_POWER_UNLOCK,
} al_power_cause_t;

al_power_state_t al_power_get();

void al_power_off();
al_power_cause_t al_power_sleep(bool deep, uint64_t timeout);
al_power_cause_t al_power_cause();

void al_power_ship();

#endif  // AL_POWER_H
