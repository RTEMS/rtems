/*
 *  Watchdog Handler
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _Watchdog_Adjust
 *
 *  This routine adjusts the delta chain backward or forward in response
 *  to a time change.
 *
 *  Input parameters:
 *    header    - pointer to the delta chain to be adjusted
 *    direction - forward or backward adjustment to delta chain
 *    units     - units to adjust
 *
 *  Output parameters:
 */

void _Watchdog_Adjust(
  Chain_Control               *header,
  Watchdog_Adjust_directions   direction,
  Watchdog_Interval            units
)
{
  if ( !_Chain_Is_empty( header ) ) {
    switch ( direction ) {
      case WATCHDOG_BACKWARD:
        _Watchdog_First( header )->delta_interval += units;
        break;
      case WATCHDOG_FORWARD:
        while ( units ) {
          if ( units < _Watchdog_First( header )->delta_interval ) {
            _Watchdog_First( header )->delta_interval -= units;
            break;
          } else {
            units -= _Watchdog_First( header )->delta_interval;
            _Watchdog_First( header )->delta_interval = 1;
            _Watchdog_Tickle( header );
            if ( _Chain_Is_empty( header ) )
              break;
          }
        }
        break;
    }
  }
}

