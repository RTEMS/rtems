/**
 * @file
 *
 * @ingroup ScoreWatchdog
 * @brief Watchdog Tickle
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

void _Watchdog_Tickle(
  Watchdog_Header *header
)
{
  ISR_lock_Context lock_context;

  _Watchdog_Acquire( header, &lock_context );

  if ( !_Watchdog_Is_empty( header ) ) {
    Watchdog_Control  *first;
    Watchdog_Interval  delta;

    first = _Watchdog_First( header );
    delta = first->delta_interval;

    /*
     * Although it is forbidden to insert watchdogs with a delta interval of
     * zero it is possible to observe watchdogs with a delta interval of zero
     * at this point.  For example lets have a watchdog chain of one watchdog
     * with a delta interval of one and insert a new one with an initial value
     * of one.  At the start of the insert procedure it will advance one step
     * and reduce its delta interval by one yielding zero.  Now a tick happens.
     * This will remove the watchdog on the chain and update the insert
     * iterator.  Now the insert operation continues and will insert the new
     * watchdog with a delta interval of zero.
     */
    if ( delta > 0 ) {
      --delta;
      first->delta_interval = delta;
    }

    while ( delta == 0 ) {
      bool                            run;
      Watchdog_Service_routine_entry  routine;
      Objects_Id                      id;
      void                           *user_data;

      run = ( first->state == WATCHDOG_ACTIVE );

      _Watchdog_Remove_it( header, first );

      routine = first->routine;
      id = first->id;
      user_data = first->user_data;

      _Watchdog_Release( header, &lock_context );

      if ( run ) {
        (*routine)( id, user_data );
      }

      _Watchdog_Acquire( header, &lock_context );

      if ( _Watchdog_Is_empty( header ) ) {
        break;
      }

      first = _Watchdog_First( header );
      delta = first->delta_interval;
    }
  }

  _Watchdog_Release( header, &lock_context );
}
