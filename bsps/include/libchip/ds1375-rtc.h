#ifndef DS1375_I2C_RTC_H
#define DS1375_I2C_RTC_H

/* Driver for the Maxim 1375 i2c RTC (TOD only; very simple...) */

/*
 * Authorship
 * ----------
 * This software was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#include <rtems.h>
#include <libchip/rtc.h>
#include <stdint.h>
#include <libchip/i2c-rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

int
rtc_ds1375_hw_init(struct i2c_rtc_base *base);

/*
 * BSP must supply the ds1375_rtc_ctx argument, which is i2c_rtc_base*
 * Use with the DS1375_RTC_INITIALIZER macro:
 *   struct i2c_rtc_base ctx = DS1375_RTC_INITIALIZER("/dev/i2c0", 0x68);
 *   ...
 *   DS1375_RTC_TBL_ENTRY("/dev/rtc", &ctx)
 */
 #define DS1375_RTC_TBL_ENTRY(dev_name, ds1375_rtc_ctx) \
 I2C_RTC_TBL_ENTRY(dev_name, ds1375_rtc_ctx)

#define DS1375_RTC_INITIALIZER(i2c_path, i2c_address) \
    I2C_RTC_INITIALIZER( \
        i2c_path, \
        i2c_address, \
        0, \
        I2C_RTC_ORDER_sec_min_hour_wkday_day_month_year, \
        "ds1375", \
        rtc_ds1375_hw_init)


#ifdef __cplusplus
}
#endif

#endif
