/*
 *  Rate Monotonic Manager
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
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>

/*PAGE
 *
 *  _Rate_monotonic_Timeout
 *
 *  This routine processes a period ending.  If the owning thread
 *  is waiting for the period, that thread is unblocked and the
 *  period reinitiated.  Otherwise, the period is expired.
 *  This routine is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - period id
 *
 *  Output parameters: NONE
 */

void _Rate_monotonic_Timeout(
  Objects_Id  id,
  void       *ignored
)
{
  Rate_monotonic_Control *the_period;
  Objects_Locations       location;
  Thread_Control         *the_thread;

  /*
   *  When we get here, the Timer is already off the chain so we do not
   *  have to worry about that -- hence no _Watchdog_Remove().
   */

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:  /* impossible */
    case OBJECTS_ERROR:
      break;

    case OBJECTS_LOCAL:
      the_thread = the_period->owner;
      if ( _States_Is_waiting_for_period( the_thread->current_state ) &&
            the_thread->Wait.id == the_period->Object.id ) {
        _Thread_Unblock( the_thread );
        the_period->owner_ticks_executed_at_period =
          the_thread->ticks_executed;

        the_period->time_at_period = _Watchdog_Ticks_since_boot;

        _Watchdog_Insert_ticks( &the_period->Timer, the_period->next_length );
      } else if ( the_period->state == RATE_MONOTONIC_OWNER_IS_BLOCKING ) {
        the_period->state = RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING;
        the_period->owner_ticks_executed_at_period =
          the_thread->ticks_executed;

        the_period->time_at_period = _Watchdog_Ticks_since_boot;
        _Watchdog_Insert_ticks( &the_period->Timer, the_period->next_length );
      } else
        the_period->state = RATE_MONOTONIC_EXPIRED;
      _Thread_Unnest_dispatch();
      break;
  }
}
