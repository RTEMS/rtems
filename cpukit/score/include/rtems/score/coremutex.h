/**
 * @file
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreMutex Mutex Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Mutex services used in all of the APIs supported by RTEMS.
 */
/**@{*/

/**
 *  @brief Control block used to manage each mutex.
 *
 *  The following defines the control block used to manage each mutex.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting to lock the mutex.
   */
  Thread_queue_Control    Wait_queue;

  /** This element points to the thread which is currently holding this mutex.
   *  The holder is the last thread to successfully lock the mutex and which
   *  has not unlocked it.  If the thread is not locked, there is no holder.
   */
  Thread_Control         *holder;
}   CORE_mutex_Control;

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
   * @brief The priority ceiling value for the mutex owner.
   */
  Priority_Control priority_ceiling;
} CORE_ceiling_mutex_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
