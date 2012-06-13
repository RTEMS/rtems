/*
 *  Clock Manager - Get Seconds Since Epoch
 *
 *  COPYRIGHT (c) 1989-2008.
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

rtems_status_code rtems_clock_get_seconds_since_epoch(
  rtems_interval *the_interval
)
{
  if ( !the_interval )
    return RTEMS_INVALID_ADDRESS;

  if ( !_TOD.is_set )
    return RTEMS_NOT_DEFINED;

  *the_interval = _TOD_Seconds_since_epoch();
  return RTEMS_SUCCESSFUL;
}
