/**
 * @file
 *
 * @brief Function sets scheduling policy and parameters of the thread
 * @ingroup POSIXAPI
 */

/*
 *  13.5.2 Dynamic Thread Scheduling Parameters Access,
 *         P1003.1c/Draft 10, p. 124
 */

/*  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/config.h>

int pthread_setschedparam(
  pthread_t           thread,
  int                 policy,
  struct sched_param *param
)
{
  Thread_Control                      *the_thread;
  POSIX_API_Control                   *api;
  Thread_CPU_budget_algorithms         budget_algorithm;
  Thread_CPU_budget_algorithm_callout  budget_callout;
  Objects_Locations                    location;
  int                                  rc;
  Priority_Control                     unused;

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
        _Watchdog_Remove_ticks( &api->Sporadic_timer );

      api->schedpolicy = policy;
      api->schedparam  = *param;
      api->Attributes.schedpolicy = policy;
      api->Attributes.schedparam  = *param;

      the_thread->budget_algorithm = budget_algorithm;
      the_thread->budget_callout   = budget_callout;

      switch ( api->schedpolicy ) {
        case SCHED_OTHER:
        case SCHED_FIFO:
        case SCHED_RR:
          the_thread->cpu_time_budget =
            rtems_configuration_get_ticks_per_timeslice();

          _Thread_Set_priority(
            the_thread,
            _POSIX_Priority_To_core( api->schedparam.sched_priority ),
            &unused,
            true
          );
          break;

        case SCHED_SPORADIC:
          api->ss_high_priority = api->schedparam.sched_priority;
          _Watchdog_Remove_ticks( &api->Sporadic_timer );
          _POSIX_Threads_Sporadic_budget_TSR( 0, the_thread );
          break;
      }

      _Objects_Put( &the_thread->Object );
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return ESRCH;
}
