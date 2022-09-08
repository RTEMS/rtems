/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief POSIX Priority Support
 *
 * This include file defines the interface to the POSIX priority
 * implementation.
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

#ifndef _RTEMS_POSIX_PRIORITYIMPL_H
#define _RTEMS_POSIX_PRIORITYIMPL_H

#include <rtems/score/scheduler.h>
#include <rtems/score/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_PRIORITY POSIX Priority Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Interface to the POSIX Priority Implementation.
 *
 * @{
 */

/**
 *  This is the numerically least important POSIX priority.
 */
#define POSIX_SCHEDULER_MINIMUM_PRIORITY (1)

/**
 * @brief Gets the maximum POSIX API priority for this scheduler instance.
 *
 * Such a priority is valid.  A scheduler instance may support priority values
 * that are not representable as an integer.
 *
 * @return The maximum POSIX API priority for this scheduler instance.
 */
static inline int _POSIX_Priority_Get_maximum(
  const Scheduler_Control *scheduler
)
{
  _Assert( (int) scheduler->maximum_priority > 1 );
  return (int) scheduler->maximum_priority - 1;
}

/**
 * @brief Converts the POSIX API priority to the corresponding SuperCore
 * priority and validates it.
 * 
 * According to POSIX, numerically higher values represent higher priorities.
 * Thus, SuperCore has priorities run in the opposite sense of the POSIX API.
 *
 * Let N be the maximum priority of this scheduler instance.   The SuperCore
 * priority zero is system reserved (PRIORITY_MINIMUM).  There are only
 * N - 1 POSIX API priority levels since a thread at SuperCore priority N would
 * never run because of the idle threads.  This is necessary because GNAT maps
 * the lowest Ada task priority to the lowest thread priority.  The lowest
 * priority Ada task should get to run, so there is a fundamental conflict with
 * having N priorities.
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in] priority The POSIX API priority to convert and validate.
 * @param[out] valid Indicates if the POSIX API priority is valid and a
 *   corresponding SuperCore priority in the specified scheduler instance
 *   exists.
 *
 * @return The corresponding SuperCore priority.
 */
Priority_Control _POSIX_Priority_To_core(
  const Scheduler_Control *scheduler,
  int                      priority,
  bool                    *valid
);

/**
 * @brief Converts the SuperCore priority to the corresponding POSIX API
 * priority.
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in] priority The SuperCore priority to convert.
 *
 * @return The corresponding POSIX API priority.
 */
int _POSIX_Priority_From_core(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
