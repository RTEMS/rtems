/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <libchip/chip.h>
#include <libchip/rtc.h>

#include <bsp.h>

#define ARBITRARY_DAY_OF_WEEK 1

void atsam_rtc_get_time(rtems_time_of_day *tod)
{
  Rtc *rtc = RTC;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t week;

  RTC_GetDate(rtc, &year, &month, &day, &week);
  RTC_GetTime(rtc, &hour, &minute, &second);

  tod->ticks = 0;
  tod->second = second;
  tod->minute = minute;
  tod->hour = hour;
  tod->day = day;
  tod->month = month;
  tod->year = year;
}

static void atsam_rtc_device_initialize(int minor)
{
  Rtc *rtc = RTC;

  RTC_DisableIt(rtc, 0x1F);
}

static int atsam_rtc_device_get_time(int minor, rtems_time_of_day *tod)
{
  atsam_rtc_get_time(tod);

  return 0;
}

static int atsam_rtc_device_set_time(int minor, const rtems_time_of_day *tod)
{
  Rtc *rtc = RTC;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t week;

  second = (uint8_t) tod->second;
  minute = (uint8_t) tod->minute;
  hour = (uint8_t) tod->hour;
  day = (uint8_t) tod->day;
  month = (uint8_t) tod->month;
  week = ARBITRARY_DAY_OF_WEEK;
  year = (uint16_t) tod->year;

  RTC_SetDate(rtc, year, month, day, week);
  RTC_SetTime(rtc, hour, minute, second);

  return 0;
}

static bool atsam_rtc_device_probe(int minor)
{
  return true;
}

const rtc_fns atsam_rtc_device_ops = {
  .deviceInitialize = atsam_rtc_device_initialize,
  .deviceGetTime = atsam_rtc_device_get_time,
  .deviceSetTime = atsam_rtc_device_set_time
};

rtc_tbl RTC_Table[] = {
  {
    .sDeviceName = "/dev/rtc",
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &atsam_rtc_device_ops,
    .deviceProbe = atsam_rtc_device_probe
  }
};

size_t RTC_Count = RTEMS_ARRAY_SIZE(RTC_Table);
