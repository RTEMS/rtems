/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the tod driver for a Philips pcf8563 I2C RTC |
\*===============================================================*/
/*
 * This file interfaces with the real-time clock found in a
 * Philips PCF8563 serial real-time clock chip.
 * This RTC have I2C bus interface. BSP have to provide I2C bus primitives
 * to make this driver working. getRegister and setRegister primitives is
 * not used here to avoid multiple transactions over I2C bus (each transaction
 * require significant time and error when date/time information red may
 * occurs). ulControlPort contains I2C bus number; ulDataPort contains
 * RTC I2C device address.
 *
 * Based on a ds1307 driver from:
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
#include "../tod/pcf8563.h"
#include "../include/i2c.h"

/* Convert from/to Binary-Coded Decimal representation */
#define From_BCD( _x ) ((((_x) >> 4) * 10) + ((_x) & 0x0F))
#define To_BCD( _x )   ((((_x) / 10) << 4) + ((_x) % 10))

/* pcf8563_initialize --
 *     Initialize PCF8563 real-time clock chip. If RTC is halted, this
 *     function resume counting.
 *
 * PARAMETERS:
 *     minor -- minor RTC device number
 */
void
pcf8563_initialize(int minor)
{
    i2c_message_status status;
    int try;
    uint8_t         ctrl1;
    i2c_bus_number bus;
    i2c_address addr;

    bus = RTC_Table[minor].ulCtrlPort1;
    addr = RTC_Table[minor].ulDataPort;

    /* Read SECONDS register */
    try = 0;
    do {
        status = i2c_wbrd(bus, addr, PCF8563_CONTROL1_ADR,
			  &ctrl1, sizeof(ctrl1));
        try++;
    } while ((status != I2C_SUCCESSFUL) && (try < 15));

    /* If clock is halted, reset and start the clock */
    if ((ctrl1 & PCF8563_CONTROL1_STOP) != 0)
    {
        uint8_t         start[8];
        memset(start, 0, sizeof(start));
        start[0] = PCF8563_CONTROL1_ADR;
        try = 0;
        do {
            status = i2c_write(bus, addr, start, 2);
        } while ((status != I2C_SUCCESSFUL) && (try < 15));
    }
}

/* pcf8563_get_time --
 *     read current time from PCF8563 real-time clock chip and convert it
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
pcf8563_get_time(int minor, rtems_time_of_day *time)
{
    i2c_bus_number bus;
    i2c_address addr;
    uint8_t         info[10];
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
        status = i2c_wbrd(bus, addr, PCF8563_SECOND_ADR, info, sizeof(info));
        try++;
    } while ((status != I2C_SUCCESSFUL) && (try < 10));

    if (status != I2C_SUCCESSFUL)
    {
        return -1;
    }

    v1 = info[PCF8563_YEAR_ADR-PCF8563_SECOND_ADR];
    v2 = From_BCD(v1);
    if ((info[PCF8563_MONTH_ADR-PCF8563_SECOND_ADR]
	 & PCF8563_MONTH_CENTURY) == 0) {
      time->year = 1900 + v2;
    }
    else {
      time->year = 2000 + v2;
    }

    v1 = info[PCF8563_MONTH_ADR-PCF8563_SECOND_ADR] & PCF8563_MONTH_MASK;
    time->month = From_BCD(v1);

    v1 = info[PCF8563_DAY_ADR-PCF8563_SECOND_ADR] & PCF8563_DAY_MASK;
    time->day = From_BCD(v1);

    v1 = info[PCF8563_HOUR_ADR-PCF8563_SECOND_ADR] & PCF8563_HOUR_MASK;
    time->hour = From_BCD(v1);

    v1 = info[PCF8563_MINUTE_ADR-PCF8563_SECOND_ADR] & PCF8563_MINUTE_MASK;
    time->minute = From_BCD(v1);

    v1 = info[PCF8563_SECOND_ADR-PCF8563_SECOND_ADR] & PCF8563_SECOND_MASK;
    time->second = From_BCD(v1);

    return 0;
}

/* pcf8563_set_time --
 *     set time to the PCF8563 real-time clock chip
 *
 * PARAMETERS:
 *     minor -- minor RTC device number
 *     time -- new date and time to be written to PCF8563
 *
 * RETURNS:
 *     0, if time obtained successfully
 *     -1, if error occured
 */
int
pcf8563_set_time(int minor, const rtems_time_of_day *time)
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

    if ((time->year >= 2100) ||
	(time->year <  1900)) {
      rtems_fatal_error_occurred(RTEMS_INVALID_NUMBER);
    }
    info[0] = PCF8563_SECOND_ADR;
    info[1 + PCF8563_YEAR_ADR  -PCF8563_SECOND_ADR] = To_BCD(time->year % 100);
    info[1 + PCF8563_MONTH_ADR -PCF8563_SECOND_ADR] = To_BCD(time->month);
    info[1 + PCF8563_DAY_ADR   -PCF8563_SECOND_ADR] = To_BCD(time->day);
    info[1 + PCF8563_HOUR_ADR  -PCF8563_SECOND_ADR] = To_BCD(time->hour);
    info[1 + PCF8563_MINUTE_ADR-PCF8563_SECOND_ADR] = To_BCD(time->minute);
    info[1 + PCF8563_SECOND_ADR-PCF8563_SECOND_ADR] = To_BCD(time->second);
    /* Do not set day of week */
    info[1 + PCF8563_DAY_OF_WEEK_ADR-PCF8563_SECOND_ADR] = 1;

    /*
     * add century info
     */
    if (time->year >= 2000) {
      info[1 + PCF8563_MONTH_ADR -PCF8563_SECOND_ADR] |= PCF8563_MONTH_CENTURY;
    }
    /*
     * send to device
     */
    try = 0;
    do {
        status = i2c_write(bus, addr, info,sizeof(info));
        try++;
    } while ((status != I2C_SUCCESSFUL) && (try < 10));

    if (status != I2C_SUCCESSFUL)
        return -1;
    else
        return 0;
}

/* Driver function table */

rtc_fns pcf8563_fns = {
    pcf8563_initialize,
    pcf8563_get_time,
    pcf8563_set_time
};
