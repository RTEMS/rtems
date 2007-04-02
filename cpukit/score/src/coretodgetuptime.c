/*
 *  Time of Day (TOD) Handler - get uptime
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
 *  _TOD_Get_uptime
 *
 *  This routine is used to obtain the system uptime
 *
 *  Input parameters:
 *    time  - pointer to the time and date structure
 *
 *  Output parameters: NONE
 */

void _TOD_Get_uptime(
  struct timespec *uptime
)
{
  ISR_Level level;
  struct timespec offset;

  /* assume uptime checked by caller */

  offset.tv_sec = 0;
  offset.tv_nsec = 0;

  _ISR_Disable( level );
    *uptime = _TOD_Uptime;
    if ( _Watchdog_Nanoseconds_since_tick_handler )
      offset.tv_nsec = (*_Watchdog_Nanoseconds_since_tick_handler)();
  _ISR_Enable( level );

  _TOD_Add_timespec( uptime, &offset );
}
