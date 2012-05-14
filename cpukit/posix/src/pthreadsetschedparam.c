/*
 *  13.5.2 Dynamic Thread Scheduling Parameters Access,
 *         P1003.1c/Draft 10, p. 124
 */

/*  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  int                                  rc;

  /*
   *  Check all the parameters
   */
  if ( !param )
    return EINVAL;

  rc = _POSIX_Thread_Translate_sched_param(
    policy,
    param,
    &budget_algorithm,
    &budget_callout
  );
  if ( rc )
    return rc;

  /*
   *  Actually change the scheduling policy and parameters
   */
  the_thread = _Thread_Get( thread, &location );
  switch ( location ) {

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
             true
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

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return ESRCH;
}
