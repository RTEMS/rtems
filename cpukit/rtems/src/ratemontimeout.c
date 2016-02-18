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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Rate_monotonic_Timeout( Watchdog_Control *watchdog )
{
  Rate_monotonic_Control *the_period;
  Thread_Control         *the_thread;

  /*
   *  When we get here, the Timer is already off the chain so we do not
   *  have to worry about that -- hence no _Watchdog_Remove().
   */
  the_period = RTEMS_CONTAINER_OF( watchdog, Rate_monotonic_Control, Timer );
  the_thread = the_period->owner;

  _Thread_Disable_dispatch();

  if ( _States_Is_waiting_for_period( the_thread->current_state ) &&
        the_thread->Wait.id == the_period->Object.id ) {
    _Thread_Unblock( the_thread );
    _Rate_monotonic_Restart( the_period );
  } else if ( the_period->state == RATE_MONOTONIC_OWNER_IS_BLOCKING ) {
    the_period->state = RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING;
    _Rate_monotonic_Restart( the_period );
  } else {
    the_period->state = RATE_MONOTONIC_EXPIRED;
  }

  _Thread_Unnest_dispatch();
}
