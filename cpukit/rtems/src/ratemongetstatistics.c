/**
 *  @file
 *
 *  @brief RTEMS Rate Monotonic Get Statistics
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

rtems_status_code rtems_rate_monotonic_get_statistics(
  rtems_id                                id,
  rtems_rate_monotonic_period_statistics *dst
)
{
  Rate_monotonic_Control          *the_period;
  ISR_lock_Context                 lock_context;
  const Rate_monotonic_Statistics *src;

  if ( dst == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_period = _Rate_monotonic_Get( id, &lock_context );
  if ( the_period == NULL ) {
    return RTEMS_INVALID_ID;
  }

  _Rate_monotonic_Acquire_critical( the_period, &lock_context );

  src = &the_period->Statistics;
  dst->count        = src->count;
  dst->missed_count = src->missed_count;
  _Timestamp_To_timespec( &src->min_cpu_time,    &dst->min_cpu_time );
  _Timestamp_To_timespec( &src->max_cpu_time,    &dst->max_cpu_time );
  _Timestamp_To_timespec( &src->total_cpu_time,  &dst->total_cpu_time );
  _Timestamp_To_timespec( &src->min_wall_time,   &dst->min_wall_time );
  _Timestamp_To_timespec( &src->max_wall_time,   &dst->max_wall_time );
  _Timestamp_To_timespec( &src->total_wall_time, &dst->total_wall_time );

  _Rate_monotonic_Release( the_period, &lock_context );
  return RTEMS_SUCCESSFUL;
}
