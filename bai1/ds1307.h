#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;

    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;

} RTC_Time;

int DS1307_ReadTime(RTC_Time *time);
int DS1307_SetTime(RTC_Time *time);

#endif
