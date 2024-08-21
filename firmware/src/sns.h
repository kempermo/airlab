#ifndef SNS_H
#define SNS_H

#define SNS_HIST 8

typedef enum {
  SNS_CO2,
  SNS_TMP,
  SNS_HUM,
  SNS_VOC,
  SNS_NOX,
} sns_mode_t;

typedef struct {
  bool ok;
  float co2;  // ppm
  float tmp;  // °C
  float hum;  // % rH
  float voc;  // indexed
  float nox;  // indexed
} sns_state_t;

typedef struct {
  float values[SNS_HIST];
  float min;
  float max;
} sns_hist_t;

void sns_init();
void sns_set(bool on);
sns_state_t sns_get();
sns_state_t sns_next();
sns_hist_t sns_query(sns_mode_t mode);

#endif  // SNS_H
