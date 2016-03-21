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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>

void _Rate_monotonic_Cancel(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  ISR_lock_Context       *lock_context
)
{
  Per_CPU_Control *cpu_self;

  _Watchdog_Per_CPU_remove_relative( &the_period->Timer );

  owner = the_period->owner;
  _Rate_monotonic_Acquire_critical( owner, lock_context );
  the_period->state = RATE_MONOTONIC_INACTIVE;

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Rate_monotonic_Release( owner, lock_context );

  _Scheduler_Release_job( owner, 0 );

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
