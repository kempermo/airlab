#ifndef DEV_H
#define DEV_H

#include <esp_attr.h>

#define DEV_KEEP RTC_DATA_ATTR

// Board Type Selection:
// 0: Single Board Prototype
// 1: AL2304-1 Boards

#define DEV_V 0

void dev_init();

#endif  // DEV_H
