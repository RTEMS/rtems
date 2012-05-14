/**
 *  @file watchdogadjusttochain.c
 *
 *  This is used by the Timer Server task.
 */

/*  COPYRIGHT (c) 1989-2009.
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

void _Watchdog_Adjust_to_chain(
  Chain_Control               *header,
  Watchdog_Interval            units_arg,
  Chain_Control               *to_fire

)
{
  Watchdog_Interval  units = units_arg;
  ISR_Level          level;
  Watchdog_Control  *first;

  if ( units <= 0 ) {
    return;
  }

  _ISR_Disable( level );

  while ( 1 ) {
    if ( units <= 0 ) {
      break;
    }
    if ( _Chain_Is_empty( header ) ) {
      break;
    }
    first = _Watchdog_First( header );

    /*
     *  If it is longer than "units" until the first element on the chain
     *  fires, then bump it and quit.
     */
    if ( units < first->delta_interval ) {
      first->delta_interval -= units;
      break;
    }

    /*
     *  The first set happens in less than units, so take all of them
     *  off the chain and adjust units to reflect this.
     */
    units -= first->delta_interval;
    first->delta_interval = 0;

    while ( 1 ) {
      _Chain_Extract_unprotected( &first->Node );
      _Chain_Append_unprotected( to_fire, &first->Node );

      _ISR_Flash( level );

      if ( _Chain_Is_empty( header ) )
        break;
      first = _Watchdog_First( header );
      if ( first->delta_interval != 0 )
        break;
    }
  }

  _ISR_Enable( level );
}

