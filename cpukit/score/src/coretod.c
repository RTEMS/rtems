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
