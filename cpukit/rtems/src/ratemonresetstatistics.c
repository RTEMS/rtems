/**
 * @file
 *
 * @brief RTEMS Rate Monotonic Reset Statistics
 * @ingroup ClassicRateMon Rate Monotonic Scheduler
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

rtems_status_code rtems_rate_monotonic_reset_statistics(
  rtems_id id
)
{
  Rate_monotonic_Control *the_period;
  ISR_lock_Context        lock_context;
  Thread_Control         *owner;

  the_period = _Rate_monotonic_Get( id, &lock_context );
  if ( the_period == NULL ) {
    return RTEMS_INVALID_ID;
  }

  owner = the_period->owner;
  _Rate_monotonic_Acquire_critical( owner, &lock_context );
  _Rate_monotonic_Reset_statistics( the_period );
  _Rate_monotonic_Release( owner, &lock_context );
  return RTEMS_SUCCESSFUL;
}
