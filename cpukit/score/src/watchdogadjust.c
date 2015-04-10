/**
 *  @file
 *
 *  @brief Watchdog Adjust
 *  @ingroup ScoreWatchdog
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/chainimpl.h>
#include <rtems/score/isrlevel.h>

void _Watchdog_Adjust_backward(
  Chain_Control     *header,
  Watchdog_Interval  units
)
{
  ISR_Level level;

  _ISR_Disable( level );

  if ( !_Chain_Is_empty( header ) ) {
     _Watchdog_First( header )->delta_interval += units;
  }

  _ISR_Enable( level );
}

void _Watchdog_Adjust_forward(
  Chain_Control     *header,
  Watchdog_Interval  units
)
{
  ISR_Level level;

  _ISR_Disable( level );

  while ( !_Chain_Is_empty( header ) && units > 0 ) {
    Watchdog_Control *first = _Watchdog_First( header );

    if ( units < first->delta_interval ) {
      first->delta_interval -= units;
      break;
    } else {
      units -= first->delta_interval;
      first->delta_interval = 1;

      _ISR_Enable( level );

      _Watchdog_Tickle( header );

      _ISR_Disable( level );
    }
  }

  _ISR_Enable( level );
}
