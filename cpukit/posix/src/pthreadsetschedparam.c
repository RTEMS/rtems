/*
 *  13.5.2 Dynamic Thread Scheduling Parameters Access, 
 *         P1003.1c/Draft 10, p. 124
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

int pthread_setschedparam(
  pthread_t           thread,
  int                 policy,
  struct sched_param *param
)
{
  register Thread_Control             *the_thread;
  POSIX_API_Control                   *api;
  Thread_CPU_budget_algorithms         budget_algorithm;
  Thread_CPU_budget_algorithm_callout  budget_callout;
  Objects_Locations                    location;
 
  /*
   *  Check all the parameters
   */

  if ( !param )
    return EINVAL;

  if ( !_POSIX_Priority_Is_valid( param->sched_priority ) )
    return EINVAL;

  budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
  budget_callout = NULL;

  switch ( policy ) {
    case SCHED_OTHER:
      budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
      break;
 
    case SCHED_FIFO:
      budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
      break;
 
    case SCHED_RR:
      budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE;
      break;
 
    case SCHED_SPORADIC:
      budget_algorithm  = THREAD_CPU_BUDGET_ALGORITHM_CALLOUT;
      budget_callout = _POSIX_Threads_Sporadic_budget_callout;
 
      if ( _POSIX_Timespec_to_interval( &param->ss_replenish_period ) <
           _POSIX_Timespec_to_interval( &param->ss_initial_budget ) )
        return EINVAL;
 
      if ( !_POSIX_Priority_Is_valid( param->ss_low_priority ) )
        return EINVAL;
 
      break;
 
    default:
      return EINVAL;
  }

  /*
   *  Actually change the scheduling policy and parameters
   */

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      return ESRCH;
    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ( api->schedpolicy == SCHED_SPORADIC )
        (void) _Watchdog_Remove( &api->Sporadic_timer );

      api->schedpolicy = policy;
      api->schedparam  = *param;
      the_thread->budget_algorithm = budget_algorithm;
      the_thread->budget_callout   = budget_callout;

      switch ( api->schedpolicy ) {
        case SCHED_OTHER:
        case SCHED_FIFO:
        case SCHED_RR:
          the_thread->cpu_time_budget = _Thread_Ticks_per_timeslice;

          the_thread->real_priority =
            _POSIX_Priority_To_core( api->schedparam.sched_priority );

          _Thread_Change_priority(
             the_thread,
             the_thread->real_priority,
             TRUE
          );
          break;
 
        case SCHED_SPORADIC:
          api->ss_high_priority = api->schedparam.sched_priority;
          _Watchdog_Remove( &api->Sporadic_timer );
          _POSIX_Threads_Sporadic_budget_TSR( 0, the_thread );
          break;
      }

      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
