/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <libchip/rtc.h>

#include <bsp/lpc24xx.h>
#include <bsp/io.h>

#define LPC24XX_RTC_NUMBER 1

static void lpc24xx_rtc_initialize(int minor)
{
  /* Enable module power */
  lpc24xx_module_enable(LPC24XX_MODULE_RTC, LPC24XX_MODULE_PCLK_DEFAULT);

  /* Enable the RTC and use external clock */
  RTC_CCR = RTC_CCR_CLKEN | RTC_CCR_CLKSRC;

  /* Disable interrupts */
  RTC_CIIR = 0;
  RTC_CISS = 0;
  RTC_AMR = 0xff;

  /* Clear interrupts */
  RTC_ILR = RTC_ILR_RTCCIF | RTC_ILR_RTCALF | RTC_ILR_RTSSF;
}

static int lpc24xx_rtc_get_time(int minor, rtems_time_of_day *tod)
{
  tod->ticks = 0;
  tod->second = RTC_SEC;
  tod->minute = RTC_MIN;
  tod->hour = RTC_HOUR;
  tod->day = RTC_DOM;
  tod->month = RTC_MONTH;
  tod->year = RTC_YEAR;

  return 0;
}

static int lpc24xx_rtc_set_time(int minor, const rtems_time_of_day *tod)
{
  RTC_SEC = tod->second;
  RTC_MIN = tod->minute;
  RTC_HOUR = tod->hour;
  RTC_DOM = tod->day;
  RTC_MONTH = tod->month;
  RTC_YEAR = tod->year;

  return 0;
}

static bool lpc24xx_rtc_probe(int minor)
{
  return true;
}

const rtc_fns lpc24xx_rtc_ops = {
  .deviceInitialize = lpc24xx_rtc_initialize,
  .deviceGetTime = lpc24xx_rtc_get_time,
  .deviceSetTime = lpc24xx_rtc_set_time
};

size_t RTC_Count = LPC24XX_RTC_NUMBER;

rtems_device_minor_number RTC_Minor = 0;

rtc_tbl	RTC_Table [LPC24XX_RTC_NUMBER] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &lpc24xx_rtc_ops,
    .deviceProbe = lpc24xx_rtc_probe,
    .pDeviceParams = NULL,
    .ulCtrlPort1 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL
  }
};
