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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Sporadic_timer_insert(
  Thread_Control    *the_thread,
  POSIX_API_Control *api
)
{
  the_thread->cpu_time_budget =
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
 * @brief POSIX threads sporadic budget callout.
 *
 * This routine handles the sporadic scheduling algorithm.
 *
 * @param[in] the_thread is a pointer to the thread whose budget
 * has been exceeded.
 */
void _POSIX_Threads_Sporadic_budget_callout(
  Thread_Control *the_thread
);

int _POSIX_Thread_Translate_to_sched_policy(
  Thread_CPU_budget_algorithms budget_algorithm
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

RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Allocate(void)
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
