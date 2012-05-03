/*  Real Time Clock Driver for Blackfin
 *
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <rtems.h>
#include "tod.h"
#include <rtems/rtc.h>
#include <rtems/libio.h>
#include <bsp.h>
#include <libcpu/rtcRegs.h>

/* The following are inside RTEMS -- we are violating visibility!!!
 * Perhaps an API could be defined to get days since 1 Jan.
 */
extern const uint16_t   _TOD_Days_to_date[2][13];

/*
 *  Prototypes and routines used below
 */
int Leap_years_until_now (int year);

void Init_RTC(void)
{
  *((uint16_t*)RTC_PREN)    = RTC_PREN_PREN; /* Enable Prescaler */
}

/*
 *  Read time from RTEMS' clock manager and set it to RTC
 */

void setRealTimeFromRTEMS (void)
{
  rtems_time_of_day time_buffer;
  rtems_status_code status;

  status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time_buffer );
  if (status == RTEMS_SUCCESSFUL){
    setRealTime(&time_buffer);
  }
}

/*
 *  Read real time from RTC and set it to RTEMS' clock manager
 */

void setRealTimeToRTEMS (void)
{
  rtems_time_of_day time_buffer;

  getRealTime(&time_buffer);
  rtems_clock_set( &time_buffer );
}

 /*
  * Set the RTC time
  */
int setRealTime(
  const rtems_time_of_day *tod
)
{
  uint32_t days;
  rtems_time_of_day tod_temp;

  tod_temp = *tod;

  days = (tod_temp.year - TOD_BASE_YEAR) * 365 + \
          _TOD_Days_to_date[0][tod_temp.month] + tod_temp.day - 1;
  if (tod_temp.month < 3)
    days +=  Leap_years_until_now (tod_temp.year - 1);
  else
    days +=  Leap_years_until_now (tod_temp.year);

  *((uint32_t volatile *)RTC_STAT) = (days << RTC_STAT_DAYS_SHIFT)|
                                     (tod_temp.hour << RTC_STAT_HOURS_SHIFT)|
                                     (tod_temp.minute << RTC_STAT_MINUTES_SHIFT)|
                                     tod_temp.second;

  return 0;
}

 /*
  *  Get the time from the RTC.
  */

void getRealTime(
  rtems_time_of_day *tod
)
{
  uint32_t days, rtc_reg;
  rtems_time_of_day tod_temp = { 0, 0, 0 };
  int n, Leap_year;

  rtc_reg = *((uint32_t volatile *)RTC_STAT);

  days = (rtc_reg >> RTC_STAT_DAYS_SHIFT) + 1;

  /* finding year */
  tod_temp.year = days/365 + TOD_BASE_YEAR;
  if (days%365 >  Leap_years_until_now (tod_temp.year - 1)) {
    days = (days%365) -  Leap_years_until_now (tod_temp.year - 1);
  } else {
    tod_temp.year--;
    days = (days%365) + 365 -  Leap_years_until_now (tod_temp.year - 1);
  }

  /* finding month and day */
  Leap_year = (((!(tod_temp.year%4)) && (tod_temp.year%100)) ||
              (!(tod_temp.year%400)))?1:0;
  for (n=1; n<=12; n++) {
    if (days <= _TOD_Days_to_date[Leap_year][n+1]) {
      tod_temp.month = n;
      tod_temp.day = days - _TOD_Days_to_date[Leap_year][n];
      break;
    }
  }

  tod_temp.hour  = (rtc_reg & RTC_STAT_HOURS_MASK) >> RTC_STAT_HOURS_SHIFT;
  tod_temp.minute  = (rtc_reg & RTC_STAT_MINUTES_MASK) >> RTC_STAT_MINUTES_SHIFT;
  tod_temp.second  = (rtc_reg & RTC_STAT_SECONDS_MASK);
  tod_temp.ticks = 0;
  *tod = tod_temp;
}

/*
 *  Return the difference between RTC and RTEMS' clock manager time in minutes.
 *  If the difference is greater than 1 day, this returns 9999.
 */

int checkRealTime (void)
{
  rtems_time_of_day rtems_tod;
  rtems_time_of_day rtc_tod;
  uint32_t   rtems_time;
  uint32_t   rtc_time;

  rtems_clock_get( RTEMS_CLOCK_GET_TOD, &rtems_tod );
  getRealTime ( &rtc_tod );

  rtems_time = _TOD_To_seconds( &rtems_tod );
  rtc_time = _TOD_To_seconds( &rtc_tod );

  return rtems_time - rtc_time;
}

int Leap_years_until_now (int year)
{
  return ((year/4 - year/100 + year/400) -
         ((TOD_BASE_YEAR - 1)/4 - (TOD_BASE_YEAR - 1)/100 +
          (TOD_BASE_YEAR - 1)/400));
}

rtems_device_driver rtc_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
{
  rtems_status_code          status;

  /*
   *  Register and initialize the primary RTC's
   */

  status = rtems_io_register_name( RTC_DEVICE_NAME, major, 0 );
  if (status != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred(status);
  }

  Init_RTC();

  setRealTimeToRTEMS();
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw = arg;
  rtems_time_of_day *tod = (rtems_time_of_day *) rw->buffer;

  rw->offset = 0;
  rw->bytes_moved = 0;

  if (rw->count != sizeof( rtems_time_of_day)) {
    return RTEMS_INVALID_SIZE;
  }

  getRealTime( tod);

  rw->bytes_moved = rw->count;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  int rv = 0;
  rtems_libio_rw_args_t *rw = arg;
  const rtems_time_of_day *tod = (const rtems_time_of_day *) rw->buffer;

  rw->offset = 0;
  rw->bytes_moved = 0;

  if (rw->count != sizeof( rtems_time_of_day)) {
    return RTEMS_INVALID_SIZE;
  }

  rv = setRealTime( tod);
  if (rv != 0) {
    return RTEMS_IO_ERROR;
  }

  rw->bytes_moved = rw->count;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver rtc_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_NOT_IMPLEMENTED;
}
