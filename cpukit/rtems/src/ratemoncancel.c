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
 *  rtems_rate_monotonic_cancel
 *
 *  This directive allows a thread to cancel a rate monotonic timer.
 *
 *  Input parameters:
 *    id - rate monotonic id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful and caller is not the owning thread
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_rate_monotonic_cancel(
  Objects_Id id
)
{
  Rate_monotonic_Control *the_period;
  Objects_Locations              location;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:            
      return RTEMS_INTERNAL_ERROR;  /* should never return this */ 

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( !_Thread_Is_executing( the_period->owner ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_NOT_OWNER_OF_RESOURCE;
      }
      (void) _Watchdog_Remove( &the_period->Timer );
      the_period->state = RATE_MONOTONIC_INACTIVE;
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
