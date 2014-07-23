/**
 * @file
 *
 * @brief Get Uptime as struct timespec
 * @ingroup ScoreTOD
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>

void _TOD_Get_uptime_as_timespec(
  struct timespec *uptime
)
{
  Timestamp_Control uptime_ts;

  /* assume time checked for NULL by caller */
  _TOD_Get_uptime( &uptime_ts );
  _Timestamp_To_timespec( &uptime_ts, uptime );
}
