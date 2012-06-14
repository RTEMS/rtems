/*
 *  Time of Day (TOD) Handler - get uptime
 */

/*  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/isr.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  _TOD_Get_uptime
 *
 *  This routine is used to obtain the system uptime
 *
 *  Input parameters:
 *    time  - pointer to the timestamp structure
 *
 *  Output parameters: NONE
 */

void _TOD_Get_uptime(
  Timestamp_Control *uptime
)
{
  ISR_Level         level;
  Timestamp_Control offset;
  Timestamp_Control up;
  long              nanoseconds;

  /* assume time checked for NULL by caller */

  /* _TOD.uptime is in native timestamp format */
  _ISR_Disable( level );
    up = _TOD.uptime;
    nanoseconds = (*_Watchdog_Nanoseconds_since_tick_handler)();
  _ISR_Enable( level );

  _Timestamp_Set( &offset, 0, nanoseconds );
  _Timestamp_Add_to( &up, &offset );
  *uptime = up;
}
