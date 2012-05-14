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

#include <unistd.h> /* getpid */
#include <sched.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/tod.h>
#include <rtems/score/thread.h>
#include <rtems/score/timespec.h>
#include <rtems/seterr.h>

int sched_rr_get_interval(
  pid_t             pid,
  struct timespec  *interval
)
{
  /*
   *  Only supported for the "calling process" (i.e. this node).
   */

  if ( pid && pid != getpid() )
    rtems_set_errno_and_return_minus_one( ESRCH );

  if ( !interval )
    rtems_set_errno_and_return_minus_one( EINVAL );

  _Timespec_From_ticks( _Thread_Ticks_per_timeslice, interval );
  return 0;
}
