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

/*
 *  _TOD_Get_uptime_as_timespec
 *
 *  This routine is used to obtain the system uptime
 *
 *  Input parameters:
 *    time  - pointer to the timestamp structure
 *
 *  Output parameters: NONE
 */

void _TOD_Get_uptime_as_timespec(
  struct timespec *uptime
)
{
  Timestamp_Control uptime_ts;

  /* assume time checked for NULL by caller */
  _TOD_Get_uptime( &uptime_ts );
  _Timestamp_To_timespec( &uptime_ts, uptime );
}
