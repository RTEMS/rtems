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
 *  @brief The blocking disciplines for a mutex.
 *
 *  This enumerated type defines the blocking disciplines for a mutex.
 */
typedef enum {
  /** This specifies that threads will wait for the mutex in FIFO order. */
  CORE_MUTEX_DISCIPLINES_FIFO,
  /** This specifies that threads will wait for the mutex in priority order.  */
  CORE_MUTEX_DISCIPLINES_PRIORITY,
  /** This specifies that threads will wait for the mutex in priority order.
   *  Additionally, the Priority Inheritance Protocol will be in effect.
   */
  CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT,
  /** This specifies that threads will wait for the mutex in priority order.
   *  Additionally, the Priority Ceiling Protocol will be in effect.
   */
  CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING
}   CORE_mutex_Disciplines;

/**
 *  @brief The possible behaviors for lock nesting.
 *
 *  This enumerated type defines the possible behaviors for
 *  lock nesting.
 */
typedef enum {
  /**
   *    This sequence has no blocking or errors:
   *
   *         + lock(m)
   *         + lock(m)
   *         + unlock(m)
   *         + unlock(m)
   */
  CORE_MUTEX_NESTING_ACQUIRES,
#if defined(RTEMS_POSIX_API)
  /**
   *    This sequence returns an error at the indicated point:
   *
   *        + lock(m)
   *        + lock(m)   - already locked error
   *        + unlock(m)
   */
  CORE_MUTEX_NESTING_IS_ERROR,
#endif
  /**
   *    This sequence performs as indicated:
   *        + lock(m)
   *        + lock(m)   - deadlocks or timeouts
   *        + unlock(m) - releases
   */
  CORE_MUTEX_NESTING_BLOCKS
}  CORE_mutex_Nesting_behaviors;

/**
 *  @brief The control block used to manage attributes of each mutex.
 *
 *  The following defines the control block used to manage the
 *  attributes of each mutex.
 */
typedef struct {
  /** This field determines what the behavior of this mutex instance will
   *  be when attempting to acquire the mutex when it is already locked.
   */
  CORE_mutex_Nesting_behaviors lock_nesting_behavior;
  /** When this field is true, then only the thread that locked the mutex
   *  is allowed to unlock it.
   */
  bool                         only_owner_release;
  /** This field indicates whether threads waiting on the mutex block in
   *  FIFO or priority order.
   */
  CORE_mutex_Disciplines       discipline;
  /** This field contains the ceiling priority to be used if that protocol
   *  is selected.
   */
  Priority_Control             priority_ceiling;
}   CORE_mutex_Attributes;

#ifdef __RTEMS_STRICT_ORDER_MUTEX__
/**
 * @brief The control block to manage lock chain of priority inheritance mutex.
 *
 * The following defines the control block used to manage lock chain of
 * priority inheritance mutex.
 */
  typedef struct{
    /** This field is a chian of locked mutex by a thread,new mutex will
     *  be added to the head of queue, and the mutex which will be released
     *  must be the head of queue.
     */
    Chain_Node                lock_queue;
    /** This field is the priority of thread before locking this mutex
     *
     */
    Priority_Control          priority_before;
  }  CORE_mutex_order_list;
#endif

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
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_mutex_Attributes   Attributes;
  /** This element contains the number of times the mutex has been acquired
   *  nested.  This must be zero (0) before the mutex is actually unlocked.
   */
  uint32_t                nest_count;
  /** This element points to the thread which is currently holding this mutex.
   *  The holder is the last thread to successfully lock the mutex and which
   *  has not unlocked it.  If the thread is not locked, there is no holder.
   */
  Thread_Control         *holder;
#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  /** This field is used to manipulate the priority inheritance mutex queue*/
  CORE_mutex_order_list   queue;
#endif

}   CORE_mutex_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
