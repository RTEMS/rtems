/*
 *  $Id$
 */

#include <sched.h>

#include <rtems/system.h>
#include <rtems/score/tod.h>
#include <rtems/score/thread.h>
#include <rtems/posix/priority.h>

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
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
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
  /*
   *  Only supported for the "calling process" (i.e. this node).
   */

  assert( pid == getpid() );

  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  13.3.4 Get Scheduling Policy, P1003.1b-1993, p. 256
 */

int sched_getscheduler(
  pid_t                     pid
)
{
  /*
   *  Only supported for the "calling process" (i.e. this node).
   */

  assert( pid == getpid() );

  return POSIX_NOT_IMPLEMENTED();
}

/*PAGE
 *
 *  13.3.6 Get Scheduling Parameter Limits, P1003.1b-1993, p. 258
 */

int sched_get_priority_max(
  int  policy
)
{
  /* XXX error check the policy */
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
  /* XXX error check the policy */
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
  time_t us_per_quantum; 

  /* XXX eventually should support different time quantums per thread */

  /* XXX should get for errors? (bad pid) */

  /* XXX some of the time routines also convert usecs to a timespec -- */
  /* XXX   should this be a common routine? */

  us_per_quantum = _TOD_Microseconds_per_tick * _Thread_Ticks_per_timeslice;

  interval->tv_sec  = us_per_quantum / TOD_MICROSECONDS_PER_SECOND;
  interval->tv_nsec = (us_per_quantum % TOD_MICROSECONDS_PER_SECOND) * 
                         TOD_NANOSECONDS_PER_MICROSECOND;
  return 0;
}

/*PAGE
 *
 *  13.3.5 Yield Processor, P1003.1b-1993, p. 257
 */

int sched_yield( void )
{
  _Thread_Yield_processor();
  return 0;
}
