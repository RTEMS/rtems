/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function sets scheduling policy and parameters of the thread
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

static int _POSIX_Set_sched_param(
  Thread_Control                       *the_thread,
  int                                   policy,
  const struct sched_param             *param,
  Thread_CPU_budget_algorithms          budget_algorithm,
  Thread_CPU_budget_algorithm_callout   budget_callout,
  Thread_queue_Context                 *queue_context
)
{
  const Scheduler_Control *scheduler;
  int                      normal_prio;
  bool                     valid;
  Priority_Control         core_normal_prio;
#if defined(RTEMS_POSIX_API)
  POSIX_API_Control       *api;
  int                      low_prio;
  Priority_Control         core_low_prio;
#endif

  normal_prio = param->sched_priority;

  scheduler = _Thread_Scheduler_get_home( the_thread );

  core_normal_prio = _POSIX_Priority_To_core( scheduler, normal_prio, &valid );
  if ( !valid ) {
    return EINVAL;
  }

#if defined(RTEMS_POSIX_API)
  if ( policy == SCHED_SPORADIC ) {
    low_prio = param->sched_ss_low_priority;
  } else {
    low_prio = normal_prio;
  }

  core_low_prio = _POSIX_Priority_To_core( scheduler, low_prio, &valid );
  if ( !valid ) {
    return EINVAL;
  }

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  _Watchdog_Per_CPU_remove_ticks( &api->Sporadic.Timer );
#endif

  _Priority_Node_set_priority( &the_thread->Real_priority, core_normal_prio );

#if defined(RTEMS_POSIX_API)
  if ( _Priority_Node_is_active( &api->Sporadic.Low_priority ) ) {
    _Thread_Priority_add(
      the_thread,
      &the_thread->Real_priority,
      queue_context
    );
    _Thread_Priority_remove(
      the_thread,
      &api->Sporadic.Low_priority,
      queue_context
    );
    _Priority_Node_set_inactive( &api->Sporadic.Low_priority );
  } else {
#endif
    _Thread_Priority_changed(
      the_thread,
      &the_thread->Real_priority,
      false,
      queue_context
    );
#if defined(RTEMS_POSIX_API)
  }
#endif

  the_thread->budget_algorithm = budget_algorithm;
  the_thread->budget_callout   = budget_callout;

#if defined(RTEMS_POSIX_API)
  _Priority_Node_set_priority( &api->Sporadic.Low_priority, core_low_prio );
  api->Sporadic.sched_ss_repl_period = param->sched_ss_repl_period;
  api->Sporadic.sched_ss_init_budget = param->sched_ss_init_budget;
  api->Sporadic.sched_ss_max_repl = param->sched_ss_max_repl;

  if ( policy == SCHED_SPORADIC ) {
    _POSIX_Threads_Sporadic_timer_insert( the_thread, api );
  } else {
#endif
    the_thread->cpu_time_budget =
      rtems_configuration_get_ticks_per_timeslice();
#if defined(RTEMS_POSIX_API)
  }
#endif

  return 0;
}

int pthread_setschedparam(
  pthread_t                 thread,
  int                       policy,
#ifdef HAVE_PTHREAD_SETSCHEDPARAM_CONST
  const struct sched_param *param
#else
  struct sched_param       *param
#endif
)
{
  Thread_CPU_budget_algorithms         budget_algorithm;
  Thread_CPU_budget_algorithm_callout  budget_callout;
  Thread_Control                      *the_thread;
  Per_CPU_Control                     *cpu_self;
  Thread_queue_Context                 queue_context;
  int                                  error;

  if ( param == NULL ) {
    return EINVAL;
  }

  error = _POSIX_Thread_Translate_sched_param(
    policy,
    param,
    &budget_algorithm,
    &budget_callout
  );
  if ( error != 0 ) {
    return error;
  }

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  the_thread = _Thread_Get( thread, &queue_context.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  _Thread_Wait_acquire_critical( the_thread, &queue_context );
  error = _POSIX_Set_sched_param(
    the_thread,
    policy,
    param,
    budget_algorithm,
    budget_callout,
    &queue_context
  );
  cpu_self = _Thread_queue_Dispatch_disable( &queue_context );
  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Priority_update( &queue_context );
  _Thread_Dispatch_enable( cpu_self );
  return error;
}
