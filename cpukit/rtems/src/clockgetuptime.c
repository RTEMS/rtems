/*
 *  Clock Manager - get uptime
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
#include <rtems/rtems/clock.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  rtems_clock_get_uptime
 *
 *  This directive obtains the system uptime.  A timestamp is the seconds
 *  and nanoseconds since boot.
 *
 *  Input parameters:
 *    timestamp - pointer to the timestamp
 *
 *  Output parameters:
 *    *uptime           - filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */
rtems_status_code rtems_clock_get_uptime(
  struct timespec *uptime
)
{
  if ( !uptime )
    return RTEMS_INVALID_ADDRESS;

  _TOD_Get_uptime_as_timespec( uptime );
  return RTEMS_SUCCESSFUL;
}
