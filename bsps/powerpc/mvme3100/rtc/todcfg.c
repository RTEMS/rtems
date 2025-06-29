/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * This file contains the RTC driver table for the MVME3100 BSP
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * Modified for mvme3100 by T. Straumann, 2007
 */

#include <bsp.h>
#include <libchip/rtc.h>
#include <libchip/ds1375-rtc.h>
#include <rtems/rtems/sem.h>

static struct i2c_rtc_base ds1375_ctx = DS1375_RTC_INITIALIZER("/dev/i2c0", 0x68);

/* The following table configures the RTC drivers used in this BSP */
rtc_tbl RTC_Table[] = {
	DS1375_RTC_TBL_ENTRY("/dev/rtc", &ds1375_ctx),
};

/* Some information used by the RTC driver */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

size_t RTC_Count = NUM_RTCS;
