/*
 *  Rate Monotonic Manager -- Get Statistics
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>

/*PAGE
 *
 *  rtems_rate_monotonic_get_statistics
 *
 *  This directive allows a thread to obtain statistics information on a
 *  period.
 *
 *  Input parameters:
 *    id         - rate monotonic id
 *    statistics - pointer to statistics control block
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 *
 */

rtems_status_code rtems_rate_monotonic_get_statistics(
  Objects_Id                               id,
  rtems_rate_monotonic_period_statistics  *statistics
)
{
  Objects_Locations                        location;
  Rate_monotonic_Control                  *the_period;
  rtems_rate_monotonic_period_statistics  *dst;
  Rate_monotonic_Statistics               *src;

  if ( !statistics )
    return RTEMS_INVALID_ADDRESS;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      dst = statistics;
      src = &the_period->Statistics;
      dst->count        = src->count;
      dst->missed_count = src->missed_count;
      #if defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
        _Timestamp_To_timespec( &src->min_cpu_time,   &dst->min_cpu_time );
        _Timestamp_To_timespec( &src->max_cpu_time,   &dst->max_cpu_time );
        _Timestamp_To_timespec( &src->total_cpu_time, &dst->total_cpu_time );
      #else
        statistics->min_wall_time   = src->min_wall_time;
        statistics->max_wall_time   = src->max_wall_time;
        statistics->total_wall_time = src->total_wall_time;
      #endif

      #if defined(RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS)
        _Timestamp_To_timespec( &src->min_wall_time,   &dst->min_wall_time );
        _Timestamp_To_timespec( &src->max_wall_time,   &dst->max_wall_time );
        _Timestamp_To_timespec( &src->total_wall_time, &dst->total_wall_time );
      #else
        dst->min_wall_time   = src->min_wall_time;
        dst->max_wall_time   = src->max_wall_time;
        dst->total_wall_time = src->total_wall_time;
      #endif

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
