/**
 * @file
 *
 * @ingroup RTEMSScoreMutex
 *
 * @brief CORE Mutex API
 *
 * This include file contains all the constants and structures associated with
 * the Mutex Handler.  A mutex is an enhanced version of the standard Dijkstra
 * binary semaphore used to provide synchronization and mutual exclusion
 * capabilities.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMUTEX_H
#define _RTEMS_SCORE_COREMUTEX_H

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/interr.h>

struct _Scheduler_Control;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreMutex Mutex Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief Mutex Handler
 *
 * This handler encapsulates functionality which provides the foundation
 * Mutex services used in all of the APIs supported by RTEMS.
 *
 * @{
 */

/**
 *  @brief Control block used to manage each mutex.
 *
 *  The following defines the control block used to manage each mutex.
 */
typedef struct {
  /**
   * @brief The thread queue of this mutex.
   *
   * The owner of the thread queue indicates the mutex owner.
   */
  Thread_queue_Control Wait_queue;
} CORE_mutex_Control;

/**
 * @brief The recursive mutex control.
 */
typedef struct {
  /**
   * @brief The plain non-recursive mutex.
   */
  CORE_mutex_Control Mutex;

  /**
   * @brief The nest level in case of a recursive seize.
   */
  unsigned int nest_level;
} CORE_recursive_mutex_Control;

/**
 * @brief The recursive mutex control with priority ceiling protocol support.
 */
typedef struct {
  /**
   * @brief The plain recursive mutex.
   */
  CORE_recursive_mutex_Control Recursive;

  /**
   * @brief The priority ceiling node for the mutex owner.
   */
  Priority_Node Priority_ceiling;

#if defined(RTEMS_SMP)
  /**
   * @brief The scheduler instance for this priority ceiling mutex.
   */
  const struct _Scheduler_Control *scheduler;
#endif
} CORE_ceiling_mutex_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
