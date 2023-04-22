#ifndef SNS_H
#define SNS_H

typedef struct {
  bool ok;
  uint16_t co2;  // ppm
  float tmp;     // °C
  float hum;     // % rH
} sns_state_t;

void sns_init();
void sns_set(bool on);
sns_state_t sns_get();
sns_state_t sns_next();

#endif  // SNS_H
