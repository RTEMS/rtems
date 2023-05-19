/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief RTC configuration.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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
