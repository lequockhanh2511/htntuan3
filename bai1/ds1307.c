#include <stdint.h>
#include "i2c.h"
#include "ds1307.h"

#define DS1307_ADDRESS 0x68

static uint8_t BCD_To_DEC(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t DEC_To_BCD(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

int DS1307_ReadTime(RTC_Time *time)
{
    uint8_t data[7];

    if (!I2C1_Read(DS1307_ADDRESS, 0x00, data, 7))
        return 0;

    time->second = BCD_To_DEC(data[0] & 0x7F);
    time->minute = BCD_To_DEC(data[1]);
    time->hour   = BCD_To_DEC(data[2] & 0x3F);

    time->day    = BCD_To_DEC(data[3]);
    time->date   = BCD_To_DEC(data[4]);
    time->month  = BCD_To_DEC(data[5]);
    time->year   = BCD_To_DEC(data[6]);

    return 1;
}

int DS1307_SetTime(RTC_Time *time)
{
    uint8_t data[7];

    data[0] = DEC_To_BCD(time->second);
    data[1] = DEC_To_BCD(time->minute);
    data[2] = DEC_To_BCD(time->hour);
    data[3] = DEC_To_BCD(time->day);
    data[4] = DEC_To_BCD(time->date);
    data[5] = DEC_To_BCD(time->month);
    data[6] = DEC_To_BCD(time->year);

    return I2C1_Write(DS1307_ADDRESS, 0x00, data, 7);
}
