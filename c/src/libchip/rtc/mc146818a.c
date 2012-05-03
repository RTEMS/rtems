/*
 *  This file interfaces with the real-time clock found in
 *  a Motorola MC146818A (common on PC hardware)
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
#include <libchip/mc146818a.h>

#define From_BCD( _x ) ((((_x) >> 4) * 10) + ((_x) & 0x0F))
#define To_BCD( _x )   ((((_x) / 10) << 4) + ((_x) % 10))

/*
 * See if chip is present
 */
bool mc146818a_probe(
  int minor
)
{
  uint32_t     mc146818a;
  getRegister_f  getReg;
  uint32_t     value;

  /*
   * Verify that chip is present and that time is valid
   */
  mc146818a = RTC_Table[ minor ].ulCtrlPort1;
  getReg = RTC_Table[ minor ].getRegister;
  value = (*getReg)( mc146818a, MC146818A_STATUSD );
  if ((value == 0) || (value == 0xFF))
    return false;
  return true;
}

/*
 * Initialize chip
 */
void mc146818a_initialize(
  int minor
)
{
  uint32_t     mc146818a;
  setRegister_f  setReg;

  mc146818a = RTC_Table[ minor ].ulCtrlPort1;
  setReg = RTC_Table[ minor ].setRegister;

  (*setReg)( mc146818a, MC146818A_STATUSA, MC146818ASA_DIVIDER|MC146818ASA_1024 );
  (*setReg)( mc146818a, MC146818A_STATUSB, MC146818ASB_24HR );
}

/*
 * Read time from chip
 */
int mc146818a_get_time(
  int                minor,
  rtems_time_of_day *time
)
{
  uint32_t     mc146818a;
  getRegister_f  getReg;
  uint32_t     value;
  rtems_interrupt_level level;

  mc146818a = RTC_Table[ minor ].ulCtrlPort1;
  getReg = RTC_Table[ minor ].getRegister;

  /*
   * No time if power failed
   */
  if (((*getReg)( mc146818a, MC146818A_STATUSD ) & MC146818ASD_PWR) == 0)
    return -1;

  /*
   * Wait for time update to complete
   */
  rtems_interrupt_disable( level );
  while (((*getReg)( mc146818a, MC146818A_STATUSA ) & MC146818ASA_TUP) != 0) {
      rtems_interrupt_flash( level );
  }

  /*
   * Read the time (we have at least 244 usec to do this)
   */
  value = (*getReg)( mc146818a, MC146818A_YEAR );
  value = From_BCD( value );
  if ( value < 88 )
    time->year = 2000 + value;
  else
    time->year = 1900 + value;

  value = (*getReg)( mc146818a, MC146818A_MONTH );
  time->month = From_BCD( value );

  value = (*getReg)( mc146818a, MC146818A_DAY );
  time->day = From_BCD( value );

  value = (*getReg)( mc146818a, MC146818A_HRS );
  time->hour = From_BCD( value );

  value = (*getReg)( mc146818a, MC146818A_MIN );
  time->minute = From_BCD( value );

  value = (*getReg)( mc146818a, MC146818A_SEC );
  rtems_interrupt_enable( level );
  time->second = From_BCD( value );
  time->ticks  = 0;

  return 0;
}

/*
 * Set time into chip
 */
int mc146818a_set_time(
  int                      minor,
  const rtems_time_of_day *time
)
{
  uint32_t     mc146818a;
  setRegister_f  setReg;

  mc146818a = RTC_Table[ minor ].ulCtrlPort1;
  setReg = RTC_Table[ minor ].setRegister;

  /*
   * Stop the RTC
   */
  (*setReg)( mc146818a, MC146818A_STATUSB, MC146818ASB_HALT|MC146818ASB_24HR );

  if ( time->year >= 2088 )
    rtems_fatal_error_occurred( RTEMS_INVALID_NUMBER );

  (*setReg)( mc146818a, MC146818A_YEAR,  To_BCD(time->year % 100) );
  (*setReg)( mc146818a, MC146818A_MONTH, To_BCD(time->month) );
  (*setReg)( mc146818a, MC146818A_DAY,   To_BCD(time->day) );
  (*setReg)( mc146818a, MC146818A_HRS,   To_BCD(time->hour) );
  (*setReg)( mc146818a, MC146818A_MIN,   To_BCD(time->minute) );
  (*setReg)( mc146818a, MC146818A_SEC,   To_BCD(time->second) );

  /*
   * Restart the RTC
   */
  (*setReg)( mc146818a, MC146818A_STATUSB, MC146818ASB_24HR );
  return 0;
}

/*
 *  Driver function table
 */
rtc_fns mc146818a_fns = {
  mc146818a_initialize,
  mc146818a_get_time,
  mc146818a_set_time
};
