/*
 *  Time of Day (TOD) Handler
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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
 *  _TOD_Handler_initialization
 *
 *  This routine initializes the time of day handler.
 *
 *  Input parameters:
 *    microseconds_per_tick - microseconds between clock ticks
 *
 *  Output parameters: NONE
 */

void _TOD_Handler_initialization(
  unsigned32 microseconds_per_tick
)
{
  _TOD_Microseconds_per_tick = microseconds_per_tick;

  _TOD_Seconds_since_epoch = 0;

  _TOD_Current.year   = TOD_BASE_YEAR;
  _TOD_Current.month  = 1;
  _TOD_Current.day    = 1;
  _TOD_Current.hour   = 0;
  _TOD_Current.minute = 0;
  _TOD_Current.second = 0;
  _TOD_Current.ticks  = 0;

  if ( microseconds_per_tick == 0 )
    _TOD_Ticks_per_second = 0;
  else
    _TOD_Ticks_per_second =
       TOD_MICROSECONDS_PER_SECOND / microseconds_per_tick;

  _Watchdog_Initialize( &_TOD_Seconds_watchdog, _TOD_Tickle, 0, NULL );

  _TOD_Is_set = FALSE;
  _TOD_Activate( _TOD_Ticks_per_second );
}

/*PAGE
 *
 *  _TOD_Set
 *
 *  This rountine sets the current date and time with the specified
 *  new date and time structure.
 *
 *  Input parameters:
 *    the_tod             - pointer to the time and date structure
 *    seconds_since_epoch - seconds since system epoch
 *
 *  Output parameters: NONE
 */

void _TOD_Set(
  TOD_Control *the_tod,
  Watchdog_Interval  seconds_since_epoch
)
{
  Watchdog_Interval ticks_until_next_second;

  _Thread_Disable_dispatch();
  _TOD_Deactivate();

  if ( seconds_since_epoch < _TOD_Seconds_since_epoch )
    _Watchdog_Adjust_seconds( WATCHDOG_BACKWARD,
       _TOD_Seconds_since_epoch - seconds_since_epoch );
  else
    _Watchdog_Adjust_seconds( WATCHDOG_FORWARD,
       seconds_since_epoch - _TOD_Seconds_since_epoch );

  ticks_until_next_second = _TOD_Ticks_per_second;
  if ( ticks_until_next_second > _TOD_Current.ticks )
    ticks_until_next_second -= _TOD_Current.ticks;

  _TOD_Current             = *the_tod;
  _TOD_Seconds_since_epoch = seconds_since_epoch;
  _TOD_Is_set              = TRUE;
  _TOD_Activate( ticks_until_next_second );

  _Thread_Enable_dispatch();
}

/*PAGE
 *
 *  _TOD_Validate
 *
 *  This kernel routine checks the validity of a date and time structure.
 *
 *  Input parameters:
 *    the_tod - pointer to a time and date structure
 *
 *  Output parameters:
 *    TRUE  - if the date, time, and tick are valid
 *    FALSE - if the the_tod is invalid
 *
 *  NOTE: This routine only works for leap-years through 2099.
 */

boolean _TOD_Validate(
  TOD_Control *the_tod
)
{
  unsigned32 days_in_month;

  if ((the_tod->ticks  >= _TOD_Ticks_per_second)  ||
      (the_tod->second >= TOD_SECONDS_PER_MINUTE) ||
      (the_tod->minute >= TOD_MINUTES_PER_HOUR)   ||
      (the_tod->hour   >= TOD_HOURS_PER_DAY)      ||
      (the_tod->month  == 0)                      ||
      (the_tod->month  >  TOD_MONTHS_PER_YEAR)    ||
      (the_tod->year   <  TOD_BASE_YEAR)          ||
      (the_tod->day    == 0) )
     return FALSE;

  if ( (the_tod->year % 4) == 0 )
    days_in_month = _TOD_Days_per_month[ 1 ][ the_tod->month ];
  else
    days_in_month = _TOD_Days_per_month[ 0 ][ the_tod->month ];

  if ( the_tod->day > days_in_month )
    return FALSE;

  return TRUE;
}

/*PAGE
 *
 *  _TOD_To_seconds
 *
 *  This routine returns the seconds from the epoch until the
 *  current date and time.
 *
 *  Input parameters:
 *    the_tod - pointer to the time and date structure
 *
 *  Output parameters:
 *    returns    - seconds since epoch until the_tod
 */

unsigned32 _TOD_To_seconds(
  TOD_Control *the_tod
)
{
  unsigned32 time;
  unsigned32 year_mod_4;

  time = the_tod->day - 1;
  year_mod_4 = the_tod->year & 3;

  if ( year_mod_4 == 0 )
    time += _TOD_Days_to_date[ 1 ][ the_tod->month ];
  else
    time += _TOD_Days_to_date[ 0 ][ the_tod->month ];

  time += ( (the_tod->year - TOD_BASE_YEAR) / 4 ) *
            ( (TOD_DAYS_PER_YEAR * 4) + 1);

  time += _TOD_Days_since_last_leap_year[ year_mod_4 ];

  time *= TOD_SECONDS_PER_DAY;

  time += ((the_tod->hour * TOD_MINUTES_PER_HOUR) + the_tod->minute)
             * TOD_SECONDS_PER_MINUTE;

  time += the_tod->second;

  return( time );
}

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
