#ifndef SYS_H
#define SYS_H

#include <stdint.h>
#include <stdbool.h>

bool sys_has_date();
void sys_get_date(uint16_t *year, uint16_t *month, uint16_t *day);
void sys_set_date(uint16_t year, uint16_t month, uint16_t day);

bool sys_has_time();
void sys_get_time(uint16_t *hour, uint16_t *minute);
void sys_set_time(uint16_t hour, uint16_t minute);

void sys_reset();

int64_t sys_get_timestamp();
void sys_conv_timestamp(int64_t ts, uint16_t *hour, uint16_t *minute, uint16_t *seconds);

#endif  // SYS_H
