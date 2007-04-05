/*
 *  Time of Day (TOD) Handler -- Tickle Ticks
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
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  _TOD_Tickle_ticks
 *
 *  This routine processes a clock tick.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 */

void _TOD_Tickle_ticks( void )
{
  struct timespec tick;
  uint32_t        seconds;

  /* Convert the tick quantum to a timespec */
  tick.tv_nsec = _TOD_Microseconds_per_tick * 1000;
  tick.tv_sec  = 0;

  /* Update the counter of ticks since boot */
  _Watchdog_Ticks_since_boot += 1;

  /* Update the timespec format uptime */
  (void) _Timespec_Add_to( &_TOD_Uptime, &tick );
  /* we do not care how much the uptime changed */

  /* Update the timespec format TOD */
  seconds = _Timespec_Add_to( &_TOD_Now, &tick );
  while ( seconds ) {
    _Watchdog_Tickle_seconds();
    seconds--;
  }
}

