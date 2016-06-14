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
  Per_CPU_Control                     *cpu_self;
  POSIX_API_Control                   *api;
  Thread_CPU_budget_algorithms         budget_algorithm;
  Thread_CPU_budget_algorithm_callout  budget_callout;
  int                                  eno;
  Priority_Control                     unused;
  ISR_lock_Context                     lock_context;
  Priority_Control                     new_priority;

  /*
   *  Check all the parameters
   */

  if ( param == NULL ) {
    return EINVAL;
  }

  eno = _POSIX_Thread_Translate_sched_param(
    policy,
    param,
    &budget_algorithm,
    &budget_callout
  );
  if ( eno != 0 ) {
    return eno;
  }

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  /*
   *  Actually change the scheduling policy and parameters
   */

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _Thread_State_acquire_critical( the_thread, &lock_context );

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  if ( api->schedpolicy == SCHED_SPORADIC ) {
    _Watchdog_Per_CPU_remove_relative( &api->Sporadic_timer );
  }

  api->schedpolicy = policy;
  api->schedparam  = *param;
  api->Attributes.schedpolicy = policy;
  api->Attributes.schedparam  = *param;

  the_thread->budget_algorithm = budget_algorithm;
  the_thread->budget_callout   = budget_callout;

  switch ( policy ) {
    case SCHED_OTHER:
    case SCHED_FIFO:
    case SCHED_RR:
      the_thread->cpu_time_budget =
        rtems_configuration_get_ticks_per_timeslice();
      new_priority = _POSIX_Priority_To_core( api->schedparam.sched_priority );
      break;
  }

  _Thread_State_release( the_thread, &lock_context );

  switch ( policy ) {
    case SCHED_OTHER:
    case SCHED_FIFO:
    case SCHED_RR:
      _Thread_Set_priority( the_thread, new_priority, &unused, false );
      break;

    case SCHED_SPORADIC:
      _POSIX_Threads_Sporadic_budget_TSR( &api->Sporadic_timer );
      break;
  }

  _Thread_Dispatch_enable( cpu_self );
  return 0;
}
