/**
 *  @file
 *
 *  @brief Watchdog Adjust to Chain
 *  @ingroup ScoreWatchdog
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/watchdogimpl.h>
#include <rtems/score/isrlevel.h>

void _Watchdog_Adjust_to_chain(
  Watchdog_Header   *header,
  Watchdog_Interval  units_arg,
  Chain_Control     *to_fire

)
{
  Watchdog_Interval  units = units_arg;
  ISR_lock_Context   lock_context;
  Watchdog_Control  *first;

  _Watchdog_Acquire( header, &lock_context );

  while ( 1 ) {
    if ( _Watchdog_Is_empty( header ) ) {
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

      _Watchdog_Flash( header, &lock_context );

      if ( _Watchdog_Is_empty( header ) )
        break;
      first = _Watchdog_First( header );
      if ( first->delta_interval != 0 )
        break;
    }
  }

  _Watchdog_Release( header, &lock_context );
}

