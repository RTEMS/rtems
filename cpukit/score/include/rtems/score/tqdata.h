/*  tqdata.h
 *
 *  This include file contains all the constants and structures
 *  needed to declare a thread queue.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __THREAD_QUEUE_DATA_h
#define __THREAD_QUEUE_DATA_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>

/*
 *  The following enumerated type details all of the disciplines
 *  supported by the Thread Queue Handler.
 */

typedef enum {
  THREAD_QUEUE_DISCIPLINE_FIFO,     /* FIFO queue discipline */
  THREAD_QUEUE_DISCIPLINE_PRIORITY  /* PRIORITY queue discipline */
}   Thread_queue_Disciplines;

/*
 *  The following enumerated types indicate what happened while the thread
 *  queue was in the synchronization window.
 */
 
typedef enum {
  THREAD_QUEUE_SYNCHRONIZED,
  THREAD_QUEUE_NOTHING_HAPPENED,
  THREAD_QUEUE_TIMEOUT,
  THREAD_QUEUE_SATISFIED
}  Thread_queue_States;

/*
 *  The following constants are used to manage the priority queues.
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
#define TASK_QUEUE_DATA_PRIORITIES_PER_HEADER      64
#define TASK_QUEUE_DATA_REVERSE_SEARCH_MASK        0x20

typedef struct {
  union {
    Chain_Control Fifo;                /* FIFO discipline list           */
    Chain_Control Priority[TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS];
                                       /* priority discipline list       */
  } Queues;
  Thread_queue_States      sync_state; /* alloc/dealloc critical section */
  Thread_queue_Disciplines discipline; /* queue discipline               */
  States_Control           state;      /* state of threads on Thread_q   */
  unsigned32               timeout_status;
}   Thread_queue_Control;

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/tqdata.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
