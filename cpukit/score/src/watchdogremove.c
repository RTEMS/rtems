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
 *  _Watchdog_Remove
 *
 *  The routine removes a watchdog from a delta chain and updates
 *  the delta counters of the remaining watchdogs.
 */

Watchdog_States _Watchdog_Remove(
  Watchdog_Control *the_watchdog
)
{
  ISR_Level         level;
  Watchdog_States   previous_state;
  Watchdog_Control *next_watchdog;

  _ISR_Disable( level );
  previous_state = the_watchdog->state;
  switch ( previous_state ) {
    case WATCHDOG_INACTIVE:
      break;

    case WATCHDOG_BEING_INSERTED:

      /*
       *  It is not actually on the chain so just change the state and
       *  the Insert operation we interrupted will be aborted.
       */
      the_watchdog->state = WATCHDOG_INACTIVE;
      break;

    case WATCHDOG_ACTIVE:
    case WATCHDOG_REMOVE_IT:

      the_watchdog->state = WATCHDOG_INACTIVE;
      next_watchdog = _Watchdog_Next( the_watchdog );

      if ( _Watchdog_Next(next_watchdog) )
        next_watchdog->delta_interval += the_watchdog->delta_interval;

      if ( _Watchdog_Sync_count )
        _Watchdog_Sync_level = _ISR_Nest_level;

      _Chain_Extract_unprotected( &the_watchdog->Node );
      break;
  }
  the_watchdog->stop_time = _Watchdog_Ticks_since_boot;

  _ISR_Enable( level );
  return( previous_state );
}
