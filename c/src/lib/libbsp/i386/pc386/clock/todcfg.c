/*
 * This file contains the RTC driver table for Motorola shared BSPs.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <libchip/rtc.h>
#include <libchip/mc146818a.h>

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
    {
        "/dev/rtc",                /* sDeviceName */
        RTC_MC146818A,             /* deviceType */
        &mc146818a_fns,            /* pDeviceFns */
        mc146818a_probe,           /* deviceProbe */
        NULL,                      /* pDeviceParams */
        0x70,                      /* ulCtrlPort1 */
        0x00,                      /* ulDataPort */
        mc146818a_get_register,    /* getRegister */
        mc146818a_set_register     /* setRegister */
    }
};

/* Some information used by the RTC driver */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

size_t RTC_Count = NUM_RTCS;

rtems_device_minor_number RTC_Minor;
