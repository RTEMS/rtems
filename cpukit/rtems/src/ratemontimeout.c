/**
 *  @file
 *
 *  @brief Rate Monotonic Timeout
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

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

    case OBJECTS_LOCAL:
      the_thread = the_period->owner;
      if ( _States_Is_waiting_for_period( the_thread->current_state ) &&
            the_thread->Wait.id == the_period->Object.id ) {
        _Thread_Unblock( the_thread );

        _Rate_monotonic_Initiate_statistics( the_period );

        _Watchdog_Insert_ticks( &the_period->Timer, the_period->next_length );
      } else if ( the_period->state == RATE_MONOTONIC_OWNER_IS_BLOCKING ) {
        the_period->state = RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING;

        _Rate_monotonic_Initiate_statistics( the_period );

        _Watchdog_Insert_ticks( &the_period->Timer, the_period->next_length );
      } else
        the_period->state = RATE_MONOTONIC_EXPIRED;
      _Objects_Put_without_thread_dispatch( &the_period->Object );
      break;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:  /* impossible */
#endif
    case OBJECTS_ERROR:
      break;
  }
}
