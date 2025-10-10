/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (C) 2009, 2011 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <libchip/rtc.h>

#include <bsp.h>
#include <bsp/lpc32xx.h>
#include <rtems/rtems/clockimpl.h>

#include <time.h>

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
  (void) minor;

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
  (void) minor;

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
  (void) minor;

  lpc32xx_rtc_set(_TOD_To_seconds(tod));

  return 0;
}

static bool lpc32xx_rtc_probe(int minor)
{
  (void) minor;

  return true;
}

const rtc_fns lpc32xx_rtc_ops = {
  .deviceInitialize = lpc32xx_rtc_initialize,
  .deviceGetTime = lpc32xx_rtc_get_time,
  .deviceSetTime = lpc32xx_rtc_set_time
};

size_t RTC_Count = LPC32XX_RTC_COUNT;

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
