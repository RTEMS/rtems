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
 *    error code        - if unsuccessful
 *
 */

rtems_status_code rtems_rate_monotonic_get_status(
  Objects_Id                           id,
  rtems_rate_monotonic_period_status  *status
)
{
  Objects_Locations              location;
  Rate_monotonic_Control        *the_period;

  if ( status == NULL )
    return RTEMS_INVALID_ADDRESS;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      status->state = the_period->state;

      if ( status->state == RATE_MONOTONIC_INACTIVE ) {
        status->ticks_since_last_period = 0;
        status->ticks_executed_since_last_period = 0;
      } else {
        status->ticks_since_last_period =
          _Watchdog_Ticks_since_boot - the_period->time_at_period;

        status->ticks_executed_since_last_period =
          the_period->owner->ticks_executed -
            the_period->owner_ticks_executed_at_period;
      }

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
