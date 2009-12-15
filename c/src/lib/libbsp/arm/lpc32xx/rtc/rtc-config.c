/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/rtc.h>

#include <bsp/lpc32xx.h>

#define LPC32XX_RTC_COUNT 1

static void lpc32xx_rtc_initialize(int minor)
{
  /* TODO */
}

static int lpc32xx_rtc_get_time(int minor, rtems_time_of_day *tod)
{
  /* TODO */

#if 0
  tod->ticks = 0;
  tod->second = RTC_SEC;
  tod->minute = RTC_MIN;
  tod->hour = RTC_HOUR;
  tod->day = RTC_DOM;
  tod->month = RTC_MONTH;
  tod->year = RTC_YEAR;
#endif

  return 0;
}

static int lpc32xx_rtc_set_time(int minor, const rtems_time_of_day *tod)
{
  /* TODO */

#if 0
  RTC_SEC = tod->second;
  RTC_MIN = tod->minute;
  RTC_HOUR = tod->hour;
  RTC_DOM = tod->day;
  RTC_MONTH = tod->month;
  RTC_YEAR = tod->year;
#endif

  return 0;
}

static bool lpc32xx_rtc_probe(int minor)
{
  return true;
}

const rtc_fns lpc32xx_rtc_ops = {
  .deviceInitialize = lpc32xx_rtc_initialize,
  .deviceGetTime = lpc32xx_rtc_get_time,
  .deviceSetTime = lpc32xx_rtc_set_time
};

unsigned long RTC_Count = LPC32XX_RTC_COUNT;

rtems_device_minor_number RTC_Minor = 0;

rtc_tbl	RTC_Table [LPC32XX_RTC_COUNT] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &lpc32xx_rtc_ops,
    .deviceProbe = lpc32xx_rtc_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
