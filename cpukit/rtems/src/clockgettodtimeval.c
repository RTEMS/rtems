/*
 *  Clock Manager - Get TOD in Time Value Format
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

rtems_status_code rtems_clock_get_tod_timeval(
  struct timeval  *time
)
{
  if ( !time )
    return RTEMS_INVALID_ADDRESS;

  if ( !_TOD.is_set )
    return RTEMS_NOT_DEFINED;

  _TOD_Get_timeval( time );

  return RTEMS_SUCCESSFUL;
}
