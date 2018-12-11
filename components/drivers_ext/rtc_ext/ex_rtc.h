#ifndef __EX_RTC_H_
#define __EX_RTC_H_
#include <stdint.h>
#include <stdbool.h>
#include "boards.h"
typedef struct  
{
    uint8_t note_time_year;
    uint8_t note_time_month;
    uint8_t note_time_day;
    uint8_t note_time_hour;
    uint8_t note_time_min;
} st_RTC_info;

void read_rtc_data(st_RTC_info * rtc_time);
void write_rtc_data(st_RTC_info * rtc_time);
uint8_t rtc_hw_check(void);

#endif
