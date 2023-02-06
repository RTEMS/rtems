/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/rtems/clock.h>
#include <rtems/score/todimpl.h>

#include <rtems/rtc.h>
#include <libchip/rtc.h>
#include <bsp/rtc.h>

extern const uint16_t _TOD_Days_to_date[2][13];

static bool _Leap_year(
  uint32_t year
)
{
  return (((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0);
}

static uint32_t _Leap_years_before(
  uint32_t year
)
{
  year -= 1;
  return (year / 4) - (year / 100) + (year / 400);
}

static uint32_t _Leap_years_between(
  uint32_t from, uint32_t to
)
{
  return _Leap_years_before( to ) - _Leap_years_before( from + 1 );
}

static uint32_t _Year_day_as_month(
  uint32_t year, uint32_t *day
)
{
  const uint16_t* days_to_date;
  uint32_t        month = 0;

  if ( _Leap_year( year ) )
    days_to_date = _TOD_Days_to_date[1];
  else
    days_to_date = _TOD_Days_to_date[0];

  days_to_date += 2;

  while (month < 11) {
    if (*day < *days_to_date)
      break;
    ++month;
    ++days_to_date;
  }

  *day -= *(days_to_date - 1);

  return month;
}

static inline void _Seconds_to_tod(uint32_t seconds, rtems_time_of_day *tod)
{
    uint32_t       days;
    uint32_t       day_secs;
    uint32_t       year;
    uint32_t       year_days;
    uint32_t       leap_years;

    /* How many days and how many seconds in the day ? */
    days = seconds / TOD_SECONDS_PER_DAY;
    day_secs = seconds % TOD_SECONDS_PER_DAY;    
    /* How many non-leap year years ? */
    year = ( days / TOD_DAYS_PER_YEAR ) + KINETIS_RTC_BASE_YEAR;    
    /* Determine the number of leap years. */
    leap_years = _Leap_years_between( KINETIS_RTC_BASE_YEAR, year );    
    /* Adjust the remaining number of days based on the leap years. */
    year_days = ( days - leap_years ) % TOD_DAYS_PER_YEAR;    
    /* Adjust the year and days in the year if in the leap year overflow. */
    if ( leap_years > ( days % TOD_DAYS_PER_YEAR ) ) {
      year -= 1;
      if ( _Leap_year( year ) ) {
        year_days += 1;
      }
    }    
    tod->year   = year;
    tod->month  = _Year_day_as_month( year, &year_days ) + 1;
    tod->day    = year_days + 1;
    tod->hour   = day_secs / (TOD_SECONDS_PER_MINUTE * TOD_MINUTES_PER_HOUR);
    tod->minute = day_secs % (TOD_SECONDS_PER_MINUTE * TOD_MINUTES_PER_HOUR);
    tod->second = tod->minute % TOD_SECONDS_PER_MINUTE;
    tod->minute = tod->minute / TOD_SECONDS_PER_MINUTE;

}

static void kinetis_rtc_get_time(rtems_time_of_day *tod)
{
    uint32_t seconds = kinetis_rtc_get_second();
    tod->ticks = 0;
    _Seconds_to_tod(seconds, tod);
}

static void kinetis_rtc_device_initialize(int minor)
{
    (void)minor;
    kinetis_rtc_init();
}

static int kinetis_rtc_device_get_time(int minor, rtems_time_of_day *tod)
{
  (void)minor;
  kinetis_rtc_get_time(tod);

  return 0;
}

static int kinetis_rtc_device_set_time(int minor, const rtems_time_of_day *tod)
{
    uint32_t seconds = _TOD_To_seconds(tod);
    kinetis_rtc_set_second(seconds);
    
    return 0;
}

static bool kinetis_rtc_device_probe(int minor)
{
  return true;
}

const rtc_fns kinetis_rtc_device_ops = {
  .deviceInitialize = kinetis_rtc_device_initialize,
  .deviceGetTime = kinetis_rtc_device_get_time,
  .deviceSetTime = kinetis_rtc_device_set_time
};

rtc_tbl RTC_Table[] = {
  {
    .sDeviceName = RTC_DEVICE_NAME,
    .deviceType = RTC_CUSTOM,
    .pDeviceFns = &kinetis_rtc_device_ops,
    .deviceProbe = kinetis_rtc_device_probe
  }
};

size_t RTC_Count = RTEMS_ARRAY_SIZE(RTC_Table);
