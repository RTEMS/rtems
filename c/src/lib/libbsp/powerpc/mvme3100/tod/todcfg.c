/*
 * This file contains the RTC driver table for the MVME3100 BSP
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * Modified for mvme3100 by T. Straumann, 2007
 */

#include <bsp.h>
#include <libchip/rtc.h>
#include <libchip/ds1375-rtc.h>

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
	DS1375_RTC_TBL_ENTRY(BSP_I2C_DS1375_RAW_DEV_NAME),
};

/* Some information used by the RTC driver */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

size_t RTC_Count = NUM_RTCS;

rtems_device_minor_number RTC_Minor;
