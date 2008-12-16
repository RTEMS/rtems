/*
 *  Time of Day (TOD) Handler
 */

/*  COPYRIGHT (c) 1989-2007.
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
  /* POSIX format TOD (timespec) */
  _Timestamp_Set( &_TOD_Now, TOD_SECONDS_1970_THROUGH_1988, 0 );

  /* Uptime (timespec) */
  _Timestamp_Set_to_zero( &_TOD_Uptime );

  /* TOD has not been set */
  _TOD_Is_set = FALSE;
  _TOD_Activate();
}
