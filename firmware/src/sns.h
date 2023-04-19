#ifndef SNS_H
#define SNS_H

typedef struct {
  uint16_t co2;  // ppm
  float tmp;     // °C
  float hum;     // % rH
} sns_state_t;

void sns_init();

void sns_set(bool on);
sns_state_t sns_get();

#endif  // SNS_H
