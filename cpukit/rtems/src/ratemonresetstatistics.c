/*
 *  Rate Monotonic Manager -- Reset Statistics
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>

/*
 *  rtems_rate_monotonic_reset_statistics
 *
 *  This directive allows a thread to reset the statistics information
 *  on a specific period instance.
 *
 *  Input parameters:
 *    id         - rate monotonic id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 *
 */

rtems_status_code rtems_rate_monotonic_reset_statistics(
  rtems_id id
)
{
  Objects_Locations              location;
  Rate_monotonic_Control        *the_period;

  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _Rate_monotonic_Reset_statistics( the_period );
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
