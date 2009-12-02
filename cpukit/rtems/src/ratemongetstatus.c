/*
 *  Rate Monotonic Manager -- Get Status
 *
 *  COPYRIGHT (c) 1989-2009.
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

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timespec.h>
#endif

/*PAGE
 *
 *  rtems_rate_monotonic_get_status
 *
 *  This directive allows a thread to obtain status information on a
 *  period.
 *
 *  Input parameters:
 *    id     - rate monotonic id
 *    status - pointer to status control block
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 *
 */

rtems_status_code rtems_rate_monotonic_get_status(
  Objects_Id                           id,
  rtems_rate_monotonic_period_status  *status
)
{
  Objects_Locations              location;
  Rate_monotonic_Control        *the_period;
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    Timestamp_Control            uptime;
    Timestamp_Control            temp;
  #endif

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
        #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
          _TOD_Get_uptime( &uptime );
          _Timestamp_Subtract( &the_period->time_at_period, &uptime, &temp );
          _Timestamp_To_timespec( &temp, &status->since_last_period );
          _Timestamp_Subtract(
            &_Thread_Time_of_last_context_switch,
            &uptime,
            &temp
          );
          _Timestamp_To_timespec( &temp, &status->executed_since_last_period );
        #else
          status->since_last_period =
            _Watchdog_Ticks_since_boot - the_period->time_at_period;
          status->executed_since_last_period =
            the_period->owner->cpu_time_used -
            the_period->owner_executed_at_period;
        #endif
      }

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
