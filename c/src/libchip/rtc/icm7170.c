/*
 *  This file interfaces with the real-time clock found in
 *  a Harris ICM7170
 *
 *  Year 2K Notes:
 *
 *  This chip only uses a two digit field to store the year.  This
 *  code uses the RTEMS Epoch as a pivot year.  This lets us map the
 *  two digit year field as follows:
 *
 *    + two digit years 0-87 are mapped to 2000-2087.
 *    + two digit years 88-99 are mapped to 1988-1999.
 *
 *  This is less than the time span supported by RTEMS.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <libchip/rtc.h>
#include <libchip/icm7170.h>

/*
 *  Control register bits
 */

/* XXX */

/*
 *  icm7170_initialize
 */

static void icm7170_initialize(
  int minor
)
{
  uintptr_t      icm7170;
  setRegister_f  setReg;
  uintptr_t      clock;

  icm7170 = RTC_Table[ minor ].ulCtrlPort1;
  setReg = RTC_Table[ minor ].setRegister;

  /*
   *  Initialize the RTC with the proper clock frequency
   */

  clock = (uintptr_t) RTC_Table[ minor ].pDeviceParams;
  (*setReg)( icm7170, ICM7170_CONTROL, 0x0c | clock );
}

/*
 *  icm7170_get_time
 */

static int icm7170_get_time(
  int                minor,
  rtems_time_of_day *time
)
{
  uint32_t       icm7170;
  getRegister_f  getReg;
  uint32_t       year;

  icm7170 = RTC_Table[ minor ].ulCtrlPort1;
  getReg = RTC_Table[ minor ].getRegister;

  /*
   *  Put the RTC into read mode
   */

  (void) (*getReg)( icm7170, ICM7170_COUNTER_HUNDREDTHS );

  /*
   *  Now get the time
   */


  year = (*getReg)( icm7170, ICM7170_YEAR );
  if ( year < 88 )
    year += 2000;
  else
    year += 1900;

  time->year   = year;
  time->month  = (*getReg)( icm7170, ICM7170_MONTH );
  time->day    = (*getReg)( icm7170, ICM7170_DATE );
  time->hour   = (*getReg)( icm7170, ICM7170_HOUR );
  time->minute = (*getReg)( icm7170, ICM7170_MINUTE );
  time->second = (*getReg)( icm7170, ICM7170_SECOND );

  time->ticks  = 0;

  /*
   *  Put the RTC back into normal mode.
   */

  (void) (*getReg)( icm7170, ICM7170_COUNTER_HUNDREDTHS );

  return 0;
}

/*
 *  icm7170_set_time
 */

static int icm7170_set_time(
  int                minor,
  const rtems_time_of_day *time
)
{
  uintptr_t      icm7170;
  setRegister_f  setReg;
  uint32_t       year;
  uintptr_t      clock;

  icm7170 = RTC_Table[ minor ].ulCtrlPort1;
  setReg = RTC_Table[ minor ].setRegister;
  clock = (uintptr_t) RTC_Table[ minor ].pDeviceParams;

  year = time->year;

  if ( year >= 2088 )
    rtems_fatal_error_occurred( RTEMS_INVALID_NUMBER );

  if ( year >= 2000 )
    year -= 2000;
  else
    year -= 1900;

  (*setReg)( icm7170, ICM7170_CONTROL, 0x04 | clock );

  (*setReg)( icm7170, ICM7170_YEAR,    year );
  (*setReg)( icm7170, ICM7170_MONTH,   time->month );
  (*setReg)( icm7170, ICM7170_DATE,    time->day );
  (*setReg)( icm7170, ICM7170_HOUR,    time->hour );
  (*setReg)( icm7170, ICM7170_MINUTE,  time->minute );
  (*setReg)( icm7170, ICM7170_SECOND,  time->second );

  /*
   *  This is not really right.
   */

  (*setReg)( icm7170, ICM7170_DAY_OF_WEEK,  1 );

  /*
   *  Put the RTC back into normal mode.
   */

  (*setReg)( icm7170, ICM7170_CONTROL, 0x0c | clock );

  return 0;
}

/*
 *  Driver function table
 */

rtc_fns icm7170_fns = {
  icm7170_initialize,
  icm7170_get_time,
  icm7170_set_time
};
