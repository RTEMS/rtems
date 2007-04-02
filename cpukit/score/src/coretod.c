/*
 *  Time of Day (TOD) Handler
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _TOD_Handler_initialization
 *
 *  This routine initializes the time of day handler.
 *
 *  Input parameters:
 *    microseconds_per_tick - microseconds between clock ticks
 *
 *  Output parameters: NONE
 */

void _TOD_Handler_initialization(
  uint32_t   microseconds_per_tick
)
{
  _TOD_Microseconds_per_tick = microseconds_per_tick;

  /* POSIX format TOD (timespec) */
  _TOD_Now.tv_sec  = TOD_SECONDS_1970_THROUGH_1988;
  _TOD_Now.tv_nsec = 0;

  /* Uptime (timespec) */
  _TOD_Uptime.tv_sec  = 0;
  _TOD_Uptime.tv_nsec = 0;

  /* Seconds since RTEMS Epoch (1988) */
  _TOD_Seconds_since_epoch = 0;

  /* Protect ourselves from a divide by zero fault */
  if ( microseconds_per_tick == 0 )
    _TOD_Ticks_per_second = 0;
  else
    _TOD_Ticks_per_second =
       TOD_MICROSECONDS_PER_SECOND / microseconds_per_tick;

  /* TOD has not been set */
  _TOD_Is_set = FALSE;
  _TOD_Activate( _TOD_Ticks_per_second );
}
