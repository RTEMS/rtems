/**
 *  @file
 *
 *  @brief Constants and Structures Needed to Declare a Thread Queue
 *
 *  This include file contains all the constants and structures
 *  needed to declare a thread queue.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADQ_H
#define _RTEMS_SCORE_THREADQ_H

#include <rtems/score/chain.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/percpu.h>
#include <rtems/score/priority.h>
#include <rtems/score/rbtree.h>
#include <rtems/score/states.h>
#include <rtems/score/threadsync.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreThreadQueue Thread Queue Handler
 *
 *  @ingroup Score
 *
 *  This handler provides the capability to have threads block in
 *  ordered sets. The sets may be ordered using the FIFO or priority
 *  discipline.
 */
/**@{*/

typedef struct Thread_queue_Control Thread_queue_Control;

/**
 * @brief Thread queue priority change operation.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_priority The new priority value.
 * @param[in] queue The thread queue.
 *
 * @see Thread_queue_Operations.
 */
typedef void ( *Thread_queue_Priority_change_operation )(
  Thread_Control       *the_thread,
  Priority_Control      new_priority,
  Thread_queue_Control *queue
);

/**
 * @brief Thread queue operations.
 *
 * @see _Thread_wait_Set_operations().
 */
typedef struct {
  /**
   * @brief Thread queue priority change operation.
   *
   * Called by _Thread_Change_priority() to notify a thread about a priority
   * change.  In case this thread waits currently for a resource the handler
   * may adjust its data structures according to the new priority value.  This
   * handler must not be NULL, instead the default handler
   * _Thread_Do_nothing_priority_change() should be used in case nothing needs
   * to be done during a priority change.
   */
  Thread_queue_Priority_change_operation priority_change;
} Thread_queue_Operations;

/**
 *  The following enumerated type details all of the disciplines
 *  supported by the Thread Queue Handler.
 */
typedef enum {
  THREAD_QUEUE_DISCIPLINE_FIFO,     /* FIFO queue discipline */
  THREAD_QUEUE_DISCIPLINE_PRIORITY  /* PRIORITY queue discipline */
}   Thread_queue_Disciplines;

/**
 *  This is the structure used to manage sets of tasks which are blocked
 *  waiting to acquire a resource.
 */
struct Thread_queue_Control {
  /** This union contains the data structures used to manage the blocked
   *  set of tasks which varies based upon the discipline.
   */
  union {
    /** This is the FIFO discipline list. */
    Chain_Control Fifo;
    /** This is the set of threads for priority discipline waiting. */
    RBTree_Control Priority;
  } Queues;

  /**
   * @brief The operations for this thread queue.
   */
  const Thread_queue_Operations *operations;

  /**
   * @brief Lock to protect this thread queue.
   *
   * It may be used to protect additional state of the object embedding this
   * thread queue.
   *
   * @see _Thread_queue_Acquire(), _Thread_queue_Acquire_critical() and
   * _Thread_queue_Release().
   */
  ISR_LOCK_MEMBER( Lock )

  /** This field is used to manage the critical section. */
  Thread_blocking_operation_States sync_state;
  /** This field indicates the thread queue's blocking discipline. */
  Thread_queue_Disciplines discipline;
  /** This is the status value returned to threads which timeout while
   *  waiting on this thread queue.
   */
  uint32_t                 timeout_status;
};

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
