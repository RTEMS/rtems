/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <sched.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/tod.h>
#include <rtems/score/thread.h>
#include <rtems/seterr.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  13.3.1 Set Scheduling Parameters, P1003.1b-1993, p. 252
 *
 */

int sched_setparam(
  pid_t                     pid,
  const struct sched_param *param
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  13.3.2 Set Scheduling Parameters, P1003.1b-1993, p. 253
 */

int sched_getparam(
  pid_t                     pid,
  const struct sched_param *param
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  13.3.3 Set Scheduling Policy and Scheduling Parameters, 
 *         P1003.1b-1993, p. 254
 */

int sched_setscheduler(
  pid_t                     pid,
  int                       policy,
  const struct sched_param *param
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  13.3.4 Get Scheduling Policy, P1003.1b-1993, p. 256
 */

int sched_getscheduler(
  pid_t                     pid
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}

/*PAGE
 *
 *  13.3.6 Get Scheduling Parameter Limits, P1003.1b-1993, p. 258
 */

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

/*PAGE
 *
 *  13.3.6 Get Scheduling Parameter Limits, P1003.1b-1993, p. 258
 */

int sched_get_priority_min(
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

  return POSIX_SCHEDULER_MINIMUM_PRIORITY;
}

/*PAGE
 *
 *  13.3.6 Get Scheduling Parameter Limits, P1003.1b-1993, p. 258
 */

int sched_rr_get_interval(
  pid_t             pid,
  struct timespec  *interval
)
{
  /* XXX do we need to support different time quantums per thread */

  /*
   *  Only supported for the "calling process" (i.e. this node).
   */

  if ( pid && pid != getpid() )
    rtems_set_errno_and_return_minus_one( ESRCH );

  if ( !interval )
    rtems_set_errno_and_return_minus_one( EINVAL );

  _POSIX_Interval_to_timespec( _Thread_Ticks_per_timeslice, interval );
  return 0;
}

/*PAGE
 *
 *  13.3.5 Yield Processor, P1003.1b-1993, p. 257
 */

int sched_yield( void )
{
  _Thread_Disable_dispatch();
    _Thread_Yield_processor();
  _Thread_Enable_dispatch();
  return 0;
}
