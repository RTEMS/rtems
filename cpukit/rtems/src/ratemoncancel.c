/**
 *  @file
 *
 *  @brief RTEMS Rate Monotonic Cancel
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>

void _Rate_monotonic_Cancel(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  ISR_lock_Context       *lock_context
)
{
  Per_CPU_Control      *cpu_self;
  Thread_queue_Context  queue_context;

  _Rate_monotonic_Acquire_critical( the_period, lock_context );

  _Watchdog_Per_CPU_remove_ticks( &the_period->Timer );
  the_period->state = RATE_MONOTONIC_INACTIVE;
  _Scheduler_Cancel_job(
    the_period->owner,
    &the_period->Priority,
    &queue_context
  );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Rate_monotonic_Release( the_period, lock_context );
  _Thread_Priority_update( &queue_context );
  _Thread_Dispatch_enable( cpu_self );
}

rtems_status_code rtems_rate_monotonic_cancel(
  rtems_id id
)
{
  Rate_monotonic_Control *the_period;
  ISR_lock_Context        lock_context;
  Thread_Control         *executing;

  the_period = _Rate_monotonic_Get( id, &lock_context );
  if ( the_period == NULL ) {
    return RTEMS_INVALID_ID;
  }

  executing = _Thread_Executing;
  if ( executing != the_period->owner ) {
    _ISR_lock_ISR_enable( &lock_context );
    return RTEMS_NOT_OWNER_OF_RESOURCE;
  }

  _Rate_monotonic_Cancel( the_period, executing, &lock_context );
  return RTEMS_SUCCESSFUL;
}
