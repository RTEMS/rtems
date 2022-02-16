/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
