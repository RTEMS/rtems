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
#include <rtems/score/states.h>
#include <rtems/score/threadsync.h>
#include <rtems/score/rbtree.h>

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
typedef struct {
  /** This union contains the data structures used to manage the blocked
   *  set of tasks which varies based upon the discipline.
   */
  union {
    /** This is the FIFO discipline list. */
    Chain_Control Fifo;
    /** This is the set of threads for priority discipline waiting. */
    RBTree_Control Priority;
  } Queues;
  /** This field is used to manage the critical section. */
  Thread_blocking_operation_States sync_state;
  /** This field indicates the thread queue's blocking discipline. */
  Thread_queue_Disciplines discipline;
  /** This indicates the blocking state for threads waiting on this
   *  thread queue.
   */
  States_Control           state;
  /** This is the status value returned to threads which timeout while
   *  waiting on this thread queue.
   */
  uint32_t                 timeout_status;
}   Thread_queue_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
