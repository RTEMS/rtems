/**
 *  @file  rtems/score/tqdata.h
 *
 *  This include file contains all the constants and structures
 *  needed to declare a thread queue.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TQDATA_H
#define _RTEMS_SCORE_TQDATA_H

/**
 *  @defgroup ScoreThreadQData Thread Queue Handler Data Definition
 *
 *  @ingroup Score
 *
 *  This handler defines the data shared between the thread and thread
 *  queue handlers.  Having this handler define these data structure
 *  avoids potentially circular references.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/threadsync.h>

/**
 *  The following enumerated type details all of the disciplines
 *  supported by the Thread Queue Handler.
 */
typedef enum {
  THREAD_QUEUE_DISCIPLINE_FIFO,     /* FIFO queue discipline */
  THREAD_QUEUE_DISCIPLINE_PRIORITY  /* PRIORITY queue discipline */
}   Thread_queue_Disciplines;

/**
 *  This is one of the constants used to manage the priority queues.
 *
 *  There are four chains used to maintain a priority -- each chain
 *  manages a distinct set of task priorities.  The number of chains
 *  is determined by TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS.
 *  The following set must be consistent.
 *
 *  The set below configures 4 headers -- each contains 64 priorities.
 *  Header x manages priority range (x*64) through ((x*64)+63).  If
 *  the priority is more than half way through the priority range it
 *  is in, then the search is performed from the rear of the chain.
 *  This halves the search time to find the insertion point.
 */
#define TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS 4

/**
 *  This is one of the constants used to manage the priority queues.
 *  @ref TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS for more details.
 */
#define TASK_QUEUE_DATA_PRIORITIES_PER_HEADER      64

/**
 *  This is one of the constants used to manage the priority queues.
 *  @ref TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS for more details.
 */
#define TASK_QUEUE_DATA_REVERSE_SEARCH_MASK        0x20

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
    /** This is the set of lists for priority discipline waiting. */
    Chain_Control Priority[TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS];
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

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/tqdata.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
