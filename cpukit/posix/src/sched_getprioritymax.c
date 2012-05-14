/*
 *  13.3.6 Get Scheduling Parameter Limits, P1003.1b-1993, p. 258
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/posix/priority.h>

int sched_get_priority_max(
  int  policy
)
{
  switch ( policy ) {
    case SCHED_OTHER:
    case SCHED_FIFO:
    case SCHED_RR:
    case SCHED_SPORADIC:
      break;

    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  return POSIX_SCHEDULER_MAXIMUM_PRIORITY;
}
