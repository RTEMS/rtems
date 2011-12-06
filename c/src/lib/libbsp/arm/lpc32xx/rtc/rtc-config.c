/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <libchip/rtc.h>

#include <bsp.h>
#include <bsp/lpc32xx.h>

#define LPC32XX_RTC_COUNT 1U

#define LPC32XX_RTC_COUNTER_DELTA 0xfffffffeU

#define LPC32XX_RTC_KEY 0xb5c13f27U

#define LPC32XX_RTC_CTRL_FORCE_ONSW (1U << 7)
#define LPC32XX_RTC_CTRL_STOP (1U << 6)
#define LPC32XX_RTC_CTRL_RESET (1U << 4)
#define LPC32XX_RTC_CTRL_MATCH_1_ONSW (1U << 3)
#define LPC32XX_RTC_CTRL_MATCH_0_ONSW (1U << 2)
#define LPC32XX_RTC_CTRL_MATCH_1_INTR (1U << 1)
#define LPC32XX_RTC_CTRL_MATCH_0_INTR (1U << 0)

static void lpc32xx_rtc_set(uint32_t val)
{
  unsigned i = lpc32xx_arm_clk() / LPC32XX_OSCILLATOR_RTC;

  lpc32xx.rtc.ctrl |= LPC32XX_RTC_CTRL_STOP;
  lpc32xx.rtc.ucount = val;
  lpc32xx.rtc.dcount = LPC32XX_RTC_COUNTER_DELTA - val;
  lpc32xx.rtc.ctrl &= ~LPC32XX_RTC_CTRL_STOP;

  /* It needs some time before we can read the values back */
  while (i != 0) {
    __asm__ volatile ("nop");
    --i;
  }
}

static void lpc32xx_rtc_reset(void)
{
  lpc32xx.rtc.ctrl = LPC32XX_RTC_CTRL_RESET;
  lpc32xx.rtc.ctrl = 0;
  lpc32xx.rtc.key = LPC32XX_RTC_KEY;
  lpc32xx_rtc_set(0);
}

static void lpc32xx_rtc_initialize(int minor)
{
  uint32_t up_first = 0;
  uint32_t up_second = 0;
  uint32_t down_first = 0;
  uint32_t down_second = 0;

  if (lpc32xx.rtc.key != LPC32XX_RTC_KEY) {
    lpc32xx_rtc_reset();
  }

  do {
    up_first = lpc32xx.rtc.ucount;
    down_first = lpc32xx.rtc.dcount;
    up_second = lpc32xx.rtc.ucount;
    down_second = lpc32xx.rtc.dcount;
  } while (up_first != up_second || down_first != down_second);

  if (up_first + down_first != LPC32XX_RTC_COUNTER_DELTA) {
    lpc32xx_rtc_reset();
  }
}

static int lpc32xx_rtc_get_time(int minor, rtems_time_of_day *tod)
{
  struct timeval now = {
    .tv_sec = lpc32xx.rtc.ucount,
    .tv_usec = 0
  };
  struct tm time;

  gmtime_r(&now.tv_sec, &time);

  tod->year   = time.tm_year + 1900;
  tod->month  = time.tm_mon + 1;
  tod->day    = time.tm_mday;
  tod->hour   = time.tm_hour;
  tod->minute = time.tm_min;
  tod->second = time.tm_sec;
  tod->ticks  = 0;

  return RTEMS_SUCCESSFUL;
}

static int lpc32xx_rtc_set_time(int minor, const rtems_time_of_day *tod)
{
  lpc32xx_rtc_set(_TOD_To_seconds(tod));

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

size_t RTC_Count = LPC32XX_RTC_COUNT;

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
