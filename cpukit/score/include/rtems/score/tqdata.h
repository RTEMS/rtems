/*  tqdata.h
 *
 *  This include file contains all the constants and structures
 *  needed to declare a thread queue.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_THREAD_QUEUE_DATA_h
#define __RTEMS_THREAD_QUEUE_DATA_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/chain.h>
#include <rtems/priority.h>
#include <rtems/states.h>

/*
 *  The following enumerated type details all of the disciplines
 *  supported by the Thread Queue Handler.
 */

typedef enum {
  THREAD_QUEUE_DISCIPLINE_FIFO,     /* RTEMS_FIFO queue discipline */
  THREAD_QUEUE_DISCIPLINE_PRIORITY  /* RTEMS_PRIORITY queue discipline */
}   Thread_queue_Disciplines;

/*
 *  The following record defines the control block used
 *  to manage each thread.
 */

#define TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS 4   /* # of pri groups */

typedef struct {
  union {
    Chain_Control Fifo;                /* FIFO discipline list           */
    Chain_Control Priority[TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS];
                                       /* priority discipline list       */
  } Queues;
  boolean                  sync;       /* alloc/dealloc critical section */
  Thread_queue_Disciplines discipline; /* queue discipline               */
  States_Control           state;      /* state of threads on Thread_q   */
}   Thread_queue_Control;

/*
 *  _Thread_queue_Header_number
 *
 *  DESCRIPTION:
 *
 *  This function returns the index of the priority chain on which
 *  a thread of the_priority should be placed.
 */

STATIC INLINE unsigned32 _Thread_queue_Header_number (
  Priority_Control the_priority
);

/*
 *  _Thread_queue_Is_reverse_search
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_priority indicates that the
 *  enqueue search should start at the front of this priority
 *  group chain, and FALSE if the search should start at the rear.
 */

STATIC INLINE boolean _Thread_queue_Is_reverse_search (
  Priority_Control the_priority
);

#include <rtems/tqdata.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
