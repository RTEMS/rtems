/*
 *  Time of Day (TOD) Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _TOD_Tickle
 *
 *  This routine updates the calendar time and tickles the
 *  per second watchdog timer chain.
 *
 *  Input parameters:
 *    ignored - this parameter is ignored
 *
 *  Output parameters: NONE
 *
 *  NOTE: This routine only works for leap-years through 2099.
 */

void _TOD_Tickle(
  Objects_Id  id,
  void       *ignored
)
{
  unsigned32 leap;

  _TOD_Current.ticks = 0;
  ++_TOD_Seconds_since_epoch;
  if ( ++_TOD_Current.second >= TOD_SECONDS_PER_MINUTE ) {
    _TOD_Current.second = 0;
    if ( ++_TOD_Current.minute >= TOD_MINUTES_PER_HOUR ) {
      _TOD_Current.minute = 0;
      if ( ++_TOD_Current.hour >= TOD_HOURS_PER_DAY ) {
        _TOD_Current.hour = 0;
        if ( _TOD_Current.year & 0x3 ) leap = 0;
        else                           leap = 1;
        if ( ++_TOD_Current.day >
               _TOD_Days_per_month[ leap ][ _TOD_Current.month ]) {
          _TOD_Current.day = 1;
          if ( ++_TOD_Current.month > TOD_MONTHS_PER_YEAR ) {
            _TOD_Current.month = 1;
            _TOD_Current.year++;
          }
        }
      }
    }
  }

  _Watchdog_Tickle_seconds();
  _Watchdog_Insert_ticks( &_TOD_Seconds_watchdog, _TOD_Ticks_per_second );
}
