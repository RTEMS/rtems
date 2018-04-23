/*
 * Copyright (c) 2016 embedded brains GmbH
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
