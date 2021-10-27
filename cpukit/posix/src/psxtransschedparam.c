/**
 * @file
 *
 * @ingroup POSIX_PTHREAD Private POSIX Threads
 *
 * @brief Translate sched_param into SuperCore Terms
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/threadcpubudget.h>

int _POSIX_Thread_Translate_to_sched_policy(
  const Thread_CPU_budget_operations *operations
)
{
  if ( operations == NULL ) {
    return SCHED_FIFO;
  }

  if ( operations == &_Thread_CPU_budget_exhaust_timeslice ) {
    return SCHED_RR;
  }

#if defined(RTEMS_POSIX_API)
  if ( operations == &_POSIX_Threads_Sporadic_budget ) {
    return SCHED_SPORADIC;
  }
#endif

  return SCHED_OTHER;
}

int _POSIX_Thread_Translate_sched_param(
  int                       policy,
  const struct sched_param *param,
  Thread_Configuration     *config
)
{
  config->cpu_budget_operations = NULL;

  if ( policy == SCHED_FIFO ) {
    return 0;
  }

  if ( policy == SCHED_OTHER ) {
    config->cpu_budget_operations = &_Thread_CPU_budget_reset_timeslice;
    return 0;
  }

  if ( policy == SCHED_RR ) {
    config->cpu_budget_operations = &_Thread_CPU_budget_exhaust_timeslice;
    return 0;
  }

#if defined(RTEMS_POSIX_API)
  if ( policy == SCHED_SPORADIC ) {
    if ( (param->sched_ss_repl_period.tv_sec == 0) &&
         (param->sched_ss_repl_period.tv_nsec == 0) )
      return EINVAL;

    if ( (param->sched_ss_init_budget.tv_sec == 0) &&
         (param->sched_ss_init_budget.tv_nsec == 0) )
      return EINVAL;

    if ( _Timespec_To_ticks( &param->sched_ss_repl_period ) <
	 _Timespec_To_ticks( &param->sched_ss_init_budget ) )
      return EINVAL;

    config->cpu_budget_operations = &_POSIX_Threads_Sporadic_budget;
    return 0;
  }
#endif

  return EINVAL;
}
