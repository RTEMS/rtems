/*
 * This file contains the RTC driver table for Motorola MCF5206eLITE
 * ColdFire evaluation board.
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <libchip/rtc.h>
#include <ds1307.h>

/* Forward function declaration */
bool mcf5206elite_ds1307_probe(int minor);

extern rtc_fns ds1307_fns;

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
    {
        "/dev/rtc",                /* sDeviceName */
        RTC_CUSTOM,                /* deviceType */
        &ds1307_fns,               /* pDeviceFns */
        mcf5206elite_ds1307_probe, /* deviceProbe */
        NULL,                      /* pDeviceParams */
        0x00,                      /* ulCtrlPort1, for DS1307-I2C bus number */
        DS1307_I2C_ADDRESS,        /* ulDataPort, for DS1307-I2C device addr */
        NULL,                      /* getRegister - not applicable to DS1307 */
        NULL                       /* setRegister - not applicable to DS1307 */
    }
};

/* Some information used by the RTC driver */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

size_t RTC_Count = NUM_RTCS;

rtems_device_minor_number RTC_Minor;

/* mcf5206elite_ds1307_probe --
 *     RTC presence probe function. Return TRUE, if device is present.
 *     Device presence checked by probe access to RTC device over I2C bus.
 *
 * PARAMETERS:
 *     minor - minor RTC device number
 *
 * RETURNS:
 *     TRUE, if RTC device is present
 */
bool
mcf5206elite_ds1307_probe(int minor)
{
    int try = 0;
    i2c_message_status status;
    rtc_tbl *rtc;
    i2c_bus_number bus;
    i2c_address addr;

    if (minor >= NUM_RTCS)
        return false;

    rtc = RTC_Table + minor;

    bus = rtc->ulCtrlPort1;
    addr = rtc->ulDataPort;
    do {
        status = i2c_wrbyte(bus, addr, 0);
        if (status == I2C_NO_DEVICE)
            return false;
        try++;
    } while ((try < 15) && (status != I2C_SUCCESSFUL));
    if (status == I2C_SUCCESSFUL)
        return true;
    else
        return false;
}
