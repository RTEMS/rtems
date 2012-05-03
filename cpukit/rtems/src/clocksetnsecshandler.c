/*
 *  Clock Manager
 *
 *  COPYRIGHT (c) 1989-2006.
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
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  rtems_clock_set_nanoseconds_extension
 *
 *  This directive sets the BSP provided nanoseconds since last tick
 *  extension.
 *
 *  Input parameters:
 *    routine - pointer to the extension routine
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */
rtems_status_code rtems_clock_set_nanoseconds_extension(
  rtems_nanoseconds_extension_routine routine
)
{
  if ( !routine )
    return RTEMS_INVALID_ADDRESS;

  _Watchdog_Nanoseconds_since_tick_handler = routine;
  return RTEMS_SUCCESSFUL;
}
