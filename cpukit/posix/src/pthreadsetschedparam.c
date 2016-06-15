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
#include <rtems/score/schedulerimpl.h>

typedef struct {
  int                                  policy;
  const struct sched_param            *param;
  Thread_CPU_budget_algorithms         budget_algorithm;
  Thread_CPU_budget_algorithm_callout  budget_callout;
  int                                  error;
} POSIX_Set_sched_param_context;

static bool _POSIX_Set_sched_param_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority_p,
  void             *arg
)
{
  POSIX_Set_sched_param_context *context;
  const struct sched_param      *param;
  const Scheduler_Control       *scheduler;
  POSIX_API_Control             *api;
  int                            low_prio;
  int                            high_prio;
  bool                           valid;
  Priority_Control               core_low_prio;
  Priority_Control               core_high_prio;
  Priority_Control               current_priority;

  context = arg;
  param = context->param;
  scheduler = _Scheduler_Get_own( the_thread );

  if ( context->policy == SCHED_SPORADIC ) {
    low_prio = param->sched_ss_low_priority;
    high_prio = param->sched_priority;
  } else {
    low_prio = param->sched_priority;
    high_prio = low_prio;
  }

  core_low_prio = _POSIX_Priority_To_core( scheduler, low_prio, &valid );
  if ( !valid ) {
    context->error = EINVAL;
    return false;
  }

  core_high_prio = _POSIX_Priority_To_core( scheduler, high_prio, &valid );
  if ( !valid ) {
    context->error = EINVAL;
    return false;
  }

  *new_priority_p = core_high_prio;

  current_priority = the_thread->current_priority;
  the_thread->real_priority = core_high_prio;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  _Watchdog_Per_CPU_remove_relative( &api->Sporadic.Timer );

  api->Attributes.schedpolicy = context->policy;
  api->Attributes.schedparam  = *param;
  api->Sporadic.low_priority  = core_low_prio;
  api->Sporadic.high_priority = core_high_prio;

  the_thread->budget_algorithm = context->budget_algorithm;
  the_thread->budget_callout   = context->budget_callout;

  if ( context->policy == SCHED_SPORADIC ) {
    _POSIX_Threads_Sporadic_timer_insert( the_thread, api );
  } else {
    the_thread->cpu_time_budget =
      rtems_configuration_get_ticks_per_timeslice();
  }

  context->error = 0;
  return _Thread_Priority_less_than( current_priority, core_high_prio )
    || !_Thread_Owns_resources( the_thread );
}

int pthread_setschedparam(
  pthread_t           thread,
  int                 policy,
  struct sched_param *param
)
{
  Thread_Control                *the_thread;
  Per_CPU_Control               *cpu_self;
  POSIX_Set_sched_param_context  context;
  ISR_lock_Context               lock_context;
  int                            error;

  if ( param == NULL ) {
    return EINVAL;
  }

  error = _POSIX_Thread_Translate_sched_param(
    policy,
    param,
    &context.budget_algorithm,
    &context.budget_callout
  );
  if ( error != 0 ) {
    return error;
  }

  context.policy = policy;
  context.param = param;

  the_thread = _Thread_Get( thread, &lock_context );

  if ( the_thread == NULL ) {
    return ESRCH;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  _Thread_Change_priority(
    the_thread,
    0,
    &context,
    _POSIX_Set_sched_param_filter,
    false
  );

  _Thread_Dispatch_enable( cpu_self );
  return context.error;
}
