/**
 *  @file
 *
 *  @brief RTEMS Rate Monotonic Get Status
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/ratemonimpl.h>
#include <rtems/score/thread.h>

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timespec.h>
#endif

rtems_status_code rtems_rate_monotonic_get_status(
  rtems_id                            id,
  rtems_rate_monotonic_period_status *status
)
{
  Thread_CPU_usage_t             executed;
  Objects_Locations              location;
  Rate_monotonic_Period_time_t   since_last_period;
  Rate_monotonic_Control        *the_period;
  bool                           valid_status;

  if ( !status )
    return RTEMS_INVALID_ADDRESS;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      status->owner = the_period->owner->Object.id;
      status->state = the_period->state;

      /*
       *  If the period is inactive, there is no information.
       */
      if ( status->state == RATE_MONOTONIC_INACTIVE ) {
        #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
          _Timespec_Set_to_zero( &status->since_last_period );
          _Timespec_Set_to_zero( &status->executed_since_last_period );
        #else
          status->since_last_period = 0;
          status->executed_since_last_period = 0;
        #endif

      } else {

        /*
         *  Grab the current status.
         */
        valid_status =
          _Rate_monotonic_Get_status(
            the_period, &since_last_period, &executed
          );
        if (!valid_status) {
          _Objects_Put( &the_period->Object );
          return RTEMS_NOT_DEFINED;
        }

        #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
          _Timestamp_To_timespec(
            &since_last_period, &status->since_last_period
          );
          _Timestamp_To_timespec(
            &executed, &status->executed_since_last_period
          );
        #else
          status->since_last_period = since_last_period;
          status->executed_since_last_period = executed;
        #endif
      }

      _Objects_Put( &the_period->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
