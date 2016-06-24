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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
RTEMS_INLINE_ROUTINE int _POSIX_Priority_Get_maximum(
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
 * priority zero is system reserved (PRIORITY_PSEUDO_ISR).  There are only
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
