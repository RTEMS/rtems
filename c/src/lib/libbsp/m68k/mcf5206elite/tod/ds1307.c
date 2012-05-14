/*
 * This file interfaces with the real-time clock found in a
 * Dallas Semiconductor DS1307/DS1308 serial real-time clock chip.
 * This RTC have I2C bus interface. BSP have to provide I2C bus primitives
 * to make this driver working. getRegister and setRegister primitives is
 * not used here to avoid multiple transactions over I2C bus (each transaction
 * require significant time and error when date/time information red may
 * occurs). ulControlPort contains I2C bus number; ulDataPort contains
 * RTC I2C device address.
 *
 * Year 2000 Note:
 *
 * This chip only uses a two digit field to store the year. This code
 * uses the RTEMS Epoch as a pivot year. This lets us map the two digit
 * year field as follows:
 *
 *    + two digit years 00-87 are mapped to 2000-2087
 *    + two digit years 88-99 are mapped to 1988-1999
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <libchip/rtc.h>
#include <string.h>
#include "ds1307.h"
#include "i2c.h"

/* Convert from/to Binary-Coded Decimal representation */
#define From_BCD( _x ) ((((_x) >> 4) * 10) + ((_x) & 0x0F))
#define To_BCD( _x )   ((((_x) / 10) << 4) + ((_x) % 10))

/* ds1307_initialize --
 *     Initialize DS1307 real-time clock chip. If RTC is halted, this
 *     function resume counting.
 *
 * PARAMETERS:
 *     minor -- minor RTC device number
 */
void
ds1307_initialize(int minor)
{
    i2c_message_status status;
    int try;
    uint8_t         sec;
    i2c_bus_number bus;
    i2c_address addr;

    bus = RTC_Table[minor].ulCtrlPort1;
    addr = RTC_Table[minor].ulDataPort;

    /* Read SECONDS register */
    try = 0;
    do {
        status = i2c_wbrd(bus, addr, 0, &sec, sizeof(sec));
        try++;
    } while ((status != I2C_SUCCESSFUL) && (try < 15));

    /* If clock is halted, reset and start the clock */
    if ((sec & DS1307_SECOND_HALT) != 0)
    {
        uint8_t         start[8];
        memset(start, 0, sizeof(start));
        start[0] = DS1307_SECOND;
        try = 0;
        do {
            status = i2c_write(bus, addr, start, 2);
        } while ((status != I2C_SUCCESSFUL) && (try < 15));
    }
}

/* ds1307_get_time --
 *     read current time from DS1307 real-time clock chip and convert it
 *     to the rtems_time_of_day structure.
 *
 * PARAMETERS:
 *     minor -- minor RTC device number
 *     time -- place to put return value (date and time)
 *
 * RETURNS:
 *     0, if time obtained successfully
 *     -1, if error occured
 */
int
ds1307_get_time(int minor, rtems_time_of_day *time)
{
    i2c_bus_number bus;
    i2c_address addr;
    uint8_t         info[8];
    uint32_t         v1, v2;
    i2c_message_status status;
    int try;

    if (time == NULL)
        return -1;

    bus = RTC_Table[minor].ulCtrlPort1;
    addr = RTC_Table[minor].ulDataPort;

    memset(time, 0, sizeof(rtems_time_of_day));
    try = 0;
    do {
        status = i2c_wbrd(bus, addr, 0, info, sizeof(info));
        try++;
    } while ((status != I2C_SUCCESSFUL) && (try < 10));

    if (status != I2C_SUCCESSFUL)
    {
        return -1;
    }

    v1 = info[DS1307_YEAR];
    v2 = From_BCD(v1);
    if (v2 < 88)
        time->year = 2000 + v2;
    else
        time->year = 1900 + v2;

    v1 = info[DS1307_MONTH] & ~0xE0;
    time->month = From_BCD(v1);

    v1 = info[DS1307_DAY] & ~0xC0;
    time->day = From_BCD(v1);

    v1 = info[DS1307_HOUR];
    if (v1 & DS1307_HOUR_12)
    {
        v2 = v1 & ~0xE0;
        if (v1 & DS1307_HOUR_PM)
        {
            time->hour = From_BCD(v2) + 12;
        }
        else
        {
            time->hour = From_BCD(v2);
        }
    }
    else
    {
        v2 = v1 & ~0xC0;
        time->hour = From_BCD(v2);
    }

    v1 = info[DS1307_MINUTE] & ~0x80;
    time->minute = From_BCD(v1);

    v1 = info[DS1307_SECOND];
    v2 = v1 & ~0x80;
    time->second = From_BCD(v2);

    return 0;
}

/* ds1307_set_time --
 *     set time to the DS1307 real-time clock chip
 *
 * PARAMETERS:
 *     minor -- minor RTC device number
 *     time -- new date and time to be written to DS1307
 *
 * RETURNS:
 *     0, if time obtained successfully
 *     -1, if error occured
 */
int
ds1307_set_time(int minor, const rtems_time_of_day *time)
{
    i2c_bus_number bus;
    i2c_address addr;
    uint8_t         info[8];
    i2c_message_status status;
    int try;

    if (time == NULL)
        return -1;

    bus = RTC_Table[minor].ulCtrlPort1;
    addr = RTC_Table[minor].ulDataPort;

    if (time->year >= 2088)
        rtems_fatal_error_occurred(RTEMS_INVALID_NUMBER);

    info[0] = DS1307_SECOND;
    info[1 + DS1307_YEAR] = To_BCD(time->year % 100);
    info[1 + DS1307_MONTH] = To_BCD(time->month);
    info[1 + DS1307_DAY] = To_BCD(time->day);
    info[1 + DS1307_HOUR] = To_BCD(time->hour);
    info[1 + DS1307_MINUTE] = To_BCD(time->minute);
    info[1 + DS1307_SECOND] = To_BCD(time->second);
    info[1 + DS1307_DAY_OF_WEEK] = 1; /* Do not set day of week */

    try = 0;
    do {
        status = i2c_write(bus, addr, info, 8);
        try++;
    } while ((status != I2C_SUCCESSFUL) && (try < 10));

    if (status != I2C_SUCCESSFUL)
        return -1;
    else
        return 0;
}

/* Driver function table */

rtc_fns ds1307_fns = {
    ds1307_initialize,
    ds1307_get_time,
    ds1307_set_time
};
