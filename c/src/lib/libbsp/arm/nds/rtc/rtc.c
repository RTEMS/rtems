/*
 * RTEMS for Nintendo DS realtime clock driver.
 *
 * Copyright (c) 2008 by Cedric Gestes <ctaf42@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <rtems.h>
#include <libchip/rtc.h>
#include <bsp.h>
#include <nds.h>

rtems_device_minor_number RTC_Minor;
size_t RTC_Count = 1;

/*
 * probe for a rtc. we always claim to have one.
 */

bool
nds_rtc_probe (int minor)
{
  return true;
}

/*
 * initialize the nds rtc.
 */

void
nds_rtc_init (int minor)
{
  /* nothing to do here (already done in the arm7 main) */
  printk ("[+] rtc started\n");
}

/*
 * read current time from nds real-time clock chip and convert it
 * to the rtems_time_of_day structure.
 */

int
nds_rtc_get_time (int minor, rtems_time_of_day * time)
{
  time->year = 2000 + IPC->time.rtc.year;
  time->month = IPC->time.rtc.month;
  time->day = IPC->time.rtc.day;
  time->hour = IPC->time.rtc.hours;
  time->minute = IPC->time.rtc.minutes;
  time->second = IPC->time.rtc.seconds;
  time->ticks = 0;

  return 0;
}

/*
 * set time to the arm7 nds rtc.
 * NOTE: this is not supported.
 */

int
nds_rtc_set_time (int minor, const rtems_time_of_day * time)
{
  return -1;
}

/*
 * driver function table.
 */

rtc_fns nds_rtc_fns = {
  nds_rtc_init,
  nds_rtc_get_time,
  nds_rtc_set_time
};

/*
 * the following table configures the RTC drivers used in this BSP
 */

rtc_tbl RTC_Table[] = {
  {
   "/dev/rtc",                  /* sDeviceName */
   RTC_CUSTOM,                  /* deviceType */
   &nds_rtc_fns,                /* pDeviceFns */
   nds_rtc_probe,               /* deviceProbe */
   NULL,                        /* pDeviceParams */
   0,                           /* ulCtrlPort1 */
   0,                           /* ulDataPort */
   NULL,                        /* getRegister */
   NULL                         /* setRegister */
   }
};
