#ifndef DEV_H
#define DEV_H

#include <esp_attr.h>

#define DEV_KEEP RTC_DATA_ATTR

#define DEV_VERSION "0.2.0"

#define DEV_MODE false

void dev_init();

#endif  // DEV_H
