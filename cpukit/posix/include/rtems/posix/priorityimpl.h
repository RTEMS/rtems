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

#include <rtems/score/priority.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_PRIORITY POSIX Priority Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Interface to the POSIX Priority Implementation
 * 
 */
/**@{**/

/**
 * 1003.1b-1993,2.2.2.80 definition of priority, p. 19
 *
 * "Numerically higher values represent higher priorities."
 *
 * Thus, RTEMS Core has priorities run in the opposite sense of the POSIX API.
 *
 * There are only 254 posix priority levels since a task at priority level
 * 255 would never run because of the RTEMS idle task.  This is necessary
 * because GNAT maps the lowest Ada task priority to the lowest thread
 * priority.  The lowest priority Ada task should get to run, so there is
 * a fundamental conflict with having 255 priorities.
 *
 * But since RTEMS can be configured with fewer than 256 priorities,
 * we use the internal constant.
 */
#define POSIX_SCHEDULER_MAXIMUM_PRIORITY (PRIORITY_MAXIMUM - 1)


/**
 *  This is the numerically least important POSIX priority.
 */
#define POSIX_SCHEDULER_MINIMUM_PRIORITY (1)

/**
 * @brief Check if POSIX priority is valid.
 * 
 * 1003.1b-1993,2.2.2.80 definition of priority, p. 19
 *
 * "Numerically higher values represent higher priorities."
 *
 * Thus, RTEMS Core has priorities run in the opposite sense of the POSIX API.
 *
 * @param[in] priority is the priority to test
 *
 * @retval TRUE The priority is valid.
 * @retval FALSE The priority is invalid.
 */
bool _POSIX_Priority_Is_valid(
  int priority
);

/**
 * @brief Convert POSIX priority to SuperCore priority.
 *
 * This method converts a POSIX API priority into onto the corresponding
 * SuperCore value.
 *
 * @param[in] priority is the POSIX API priority.
 *
 * @return This method returns the corresponding SuperCore priority.
 */
RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Priority_To_core(
  int priority
)
{
  return (Priority_Control) (POSIX_SCHEDULER_MAXIMUM_PRIORITY - priority + 1);
}

/**
 * @brief Convert SuperCore priority To POSIX priority.
 *
 * This method converts a SuperCore priority into onto the corresponding
 * POSIX API value.
 *
 * @param[in] priority is the POSIX API priority.
 *
 * @return This method returns the corresponding POSIX priority.
 */
RTEMS_INLINE_ROUTINE int _POSIX_Priority_From_core(
  Priority_Control priority
)
{
  return (POSIX_SCHEDULER_MAXIMUM_PRIORITY - priority + 1);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
