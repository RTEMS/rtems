/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief POSIX Threads Private Support
 *
 * This include file contains all the private support information for
 * POSIX threads.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_POSIX_PTHREADIMPL_H
#define _RTEMS_POSIX_PTHREADIMPL_H

#include <rtems/posix/pthread.h>
#include <rtems/posix/threadsup.h>
#include <rtems/score/assert.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/timespec.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup POSIX_PTHREAD
 */
/**@{**/

/**
 * The following sets the minimum stack size for POSIX threads.
 */
#define PTHREAD_MINIMUM_STACK_SIZE _POSIX_Threads_Minimum_stack_size

#if defined(RTEMS_POSIX_API)
static inline void _POSIX_Threads_Sporadic_timer_insert(
  Thread_Control    *the_thread,
  POSIX_API_Control *api
)
{
  the_thread->CPU_budget.available =
    _Timespec_To_ticks( &api->Sporadic.sched_ss_init_budget );

  _Watchdog_Per_CPU_insert_ticks(
    &api->Sporadic.Timer,
    _Per_CPU_Get(),
    _Timespec_To_ticks( &api->Sporadic.sched_ss_repl_period )
  );
}
#endif

void _POSIX_Threads_Sporadic_timer( Watchdog_Control *watchdog );

/**
 * @brief The POSIX threads sporadic budget operations.
 */
extern const Thread_CPU_budget_operations _POSIX_Threads_Sporadic_budget;

int _POSIX_Thread_Translate_to_sched_policy(
  const Thread_CPU_budget_operations *operations
);

/**
 * @brief Translates the POSIX scheduling policy and parameters to parts of the
 *   thread configuration.
 *
 * @param policy is the POSIX scheduling policy.
 *
 * @param param is the pointer to the POSIX scheduling parameters.
 *
 * @param[out] config is the pointer to a thread configuration to set the
 *   budget algorithm, callout, and CPU time budget.
 *
 * @retval 0 The operation was successful.
 *
 * @retval EINVAL The POSIX scheduling policy or parameters were invalid.
 */
int _POSIX_Thread_Translate_sched_param(
  int                       policy,
  const struct sched_param *param,
  Thread_Configuration     *config
);

static inline Thread_Control *_POSIX_Threads_Allocate(void)
{
  _Objects_Allocator_lock();

  _Thread_Kill_zombies();

  return (Thread_Control *)
    _Objects_Allocate_unprotected( &_POSIX_Threads_Information.Objects );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
