/*
 * This file contains the RTC driver table for Motorola shared BSPs.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <libchip/rtc.h>
#include <libchip/m48t08.h>

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
    {
        "/dev/rtc",                /* sDeviceName */
        RTC_M48T08,                /* deviceType */
        &m48t08_fns,               /* pDeviceFns */
        rtc_probe,                 /* deviceProbe */
        NULL,                      /* pDeviceParams */
        0xF1117FF8,                /* ulCtrlPort1 */
        0x00,                      /* ulDataPort */
        m48t08_get_register,       /* getRegister */
        m48t08_set_register        /* setRegister */
    }
};

/* Some information used by the RTC driver */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

size_t RTC_Count = NUM_RTCS;

rtems_device_minor_number RTC_Minor;
