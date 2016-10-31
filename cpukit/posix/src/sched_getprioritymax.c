/**
 * @file
 *
 * @brief Returns the Appropriate Maximum for Scheduling policy
 * @ingroup POSIXAPI
 */

/*
 *  13.3.6 Get Scheduling Parameter Limits, P1003.1b-1993, p. 258
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>

#include <rtems/seterr.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>

int sched_get_priority_max(
  int  policy
)
{
  const Scheduler_Control *scheduler;

  switch ( policy ) {
    case SCHED_OTHER:
    case SCHED_FIFO:
    case SCHED_RR:
    case SCHED_SPORADIC:
      break;

    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  scheduler = _Thread_Scheduler_get_home( _Thread_Get_executing() );
  return _POSIX_Priority_Get_maximum( scheduler );
}
