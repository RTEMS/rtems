/*
 *  Watchdog Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>

/*
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
  ISR_Level level;

  _ISR_Disable( level );

  /*
   * NOTE: It is safe NOT to make 'header' a pointer
   *       to volatile data (contrast this with watchdoginsert.c)
   *       because we call _Watchdog_Tickle() below and
   *       hence the compiler must not assume *header to remain
   *       unmodified across that call.
   *
   *       Till Straumann, 7/2003
   */
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

            _ISR_Enable( level );

            _Watchdog_Tickle( header );

            _ISR_Disable( level );

            if ( _Chain_Is_empty( header ) )
              break;
          }
        }
        break;
    }
  }

  _ISR_Enable( level );

}
