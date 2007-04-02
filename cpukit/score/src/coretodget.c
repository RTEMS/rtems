/*
 *  Time of Day (TOD) Handler - get TOD
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/isr.h>
#include <rtems/score/tod.h>

/*
 *  _TOD_Get
 *
 *  This routine is used to obtain the current date and time.
 *
 *  Input parameters:
 *    time  - pointer to the time and date structure
 *
 *  Output parameters: NONE
 */

void _TOD_Get(
  struct timespec *time
)
{
  ISR_Level level;
  struct timespec offset;

  /* assume time checked by caller */

  offset.tv_sec = 0;
  offset.tv_nsec = 0;

  /* _TOD_Now is a proper POSIX time */
  _ISR_Disable( level );
    *time = _TOD_Now;
    if ( _Watchdog_Nanoseconds_since_tick_handler )
      offset.tv_nsec = (*_Watchdog_Nanoseconds_since_tick_handler)();
  _ISR_Enable( level );

  _TOD_Add_timespec( time, &offset );
}
