#ifndef AL_ULP_SHARED_H
#define AL_ULP_SHARED_H

#include <stdint.h>

typedef enum {
  AL_ULP_TYPE_ERROR
} al_ulp_log_type_t;

typedef struct {
  int32_t time; // ms
  al_ulp_log_type_t type;
  int64_t value;
} al_ulp_log_t;

#endif  // AL_ULP_SHARED_H
