/**
 *  @file  rtems/score/schedulersimple.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of threads on a simple-priority-based ready queue.
 *
 *
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_H
#define _RTEMS_SCORE_SCHEDULERSIMPLE_H

/**
 *  @addtogroup ScoreScheduler
 *
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>

/**
 *  Entry points for Scheduler Simple
 */
#define SCHEDULER_SIMPLE_ENTRY_POINTS \
  { \
    _Scheduler_simple_Initialize,         /* initialize entry point */ \
    _Scheduler_simple_Schedule,           /* schedule entry point */ \
    _Scheduler_simple_Yield,              /* yield entry point */ \
    _Scheduler_simple_Block,              /* block entry point */ \
    _Scheduler_simple_Unblock,            /* unblock entry point */ \
    _Scheduler_simple_Allocate,           /* allocate entry point */ \
    _Scheduler_simple_Free,               /* free entry point */ \
    _Scheduler_simple_Update,             /* update entry point */ \
    _Scheduler_simple_Enqueue,            /* enqueue entry point */ \
    _Scheduler_simple_Enqueue_first,      /* enqueue_first entry point */ \
    _Scheduler_simple_Extract,            /* extract entry point */ \
    _Scheduler_priority_Priority_compare, /* compares two priorities */ \
    _Scheduler_priority_Release_job,      /* new period of task */ \
    _Scheduler_priority_Tick              /* tick entry point */ \
  }

/**
 * This routine initializes the simple scheduler.
 */
void _Scheduler_simple_Initialize( void );

/**
 *  This routine sets the heir thread to be the next ready thread
 *  on the ready queue by getting the first node in the scheduler
 *  information.
 */
void _Scheduler_simple_Schedule( void );

/**
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread in the queue.
 *  It will remove the running THREAD from the scheduler.informaiton
 *  (where the ready queue is stored) and place it immediately at the
 *  between the last entry of its priority and the next priority thread.
 *  Reset timeslice and yield the processor functions both use this routine,
 *  therefore if reset is true and this is the only thread on the queue then
 *  the timeslice counter is reset.  The heir THREAD will be updated if the
 *  running is also the currently the heir.
*/
void _Scheduler_simple_Yield( void );

/**
 *  This routine removes @a the_thread from the scheduling decision,
 *  that is, removes it from the ready queue.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  @param[in] the_thread is the thread that is to be blocked
 */
void _Scheduler_simple_Block(
  Thread_Control *the_thread
);

/**
 *  This routine adds @a the_thread to the scheduling decision,
 *  that is, adds it to the ready queue and
 *  updates any appropriate scheduling variables, for example the heir thread.
 *
 *  @param[in] the_thread is the thread that is to be unblocked
 */
void _Scheduler_simple_Unblock(
  Thread_Control *the_thread
);

/**
 *  This routine removes a specific thread from the specified
 *  simple-based ready queue.
 *
 *  @param[in] the_thread is the thread to be blocked
 */
void _Scheduler_simple_Extract(
  Thread_Control *the_thread
);

/**
 *  This routine puts @a the_thread on to the ready queue.
 *
 *  @param[in] the_thread is the thread to be blocked
 */
void _Scheduler_simple_Enqueue(
  Thread_Control *the_thread
);

/**
 *  This routine puts @a the_thread to the head of the ready queue.
 *  The thread will be the first thread at its priority level.
 *
 *  @param[in] the_thread is the thread to be blocked
 */
void _Scheduler_simple_Enqueue_first(
  Thread_Control *the_thread
);

/**
 *  This routine is a place holder for any memeory allocation needed
 *  by the scheduler.  For the simple scheduler the routine is an empty
 *  place holder.
 *
 *  @param[in] the_thread is the thread the scheduler is allocating
 *             management memory for
 *
 *  @return this routine returns -1 since this is just an empty placeholder
 *  and the return value may be defined differently by each scheduler.
 */
void *_Scheduler_simple_Allocate(
  Thread_Control *the_thread
);

/**
 * This routine does nothing, and is used as a stub for Schedule update
 *
 * The overhead of a function call will still be imposed.
 *
 *  @param[in] the_thread is the thread to be blocked
 */
void _Scheduler_simple_Update(
  Thread_Control *the_thread
);

/**
 * This routine does nothing, and is used as a stub for Schedule free
 *
 * The overhead of a function call will still be imposed.
 *
 *  @param[in] the_thread is the thread to be blocked
 */
void _Scheduler_simple_Free(
  Thread_Control *the_thread
);

/**
 *  _Scheduler_simple_Ready_queue_enqueue
 *
 *  This routine puts @a the_thread on the ready queue
 *  at the end of its priority group.
 *
 *  @param[in] the_thread - pointer to a thread control block
 */
void _Scheduler_simple_Ready_queue_enqueue(
  Thread_Control    *the_thread
);

/**
 *  _Scheduler_simple_Ready_queue_enqueue_first
 *
 *  This routine puts @a the_thread on to the ready queue
 *  at the beginning of its priority group.
 *
 *  @param[in] the_thread - pointer to a thread control block
 */
void _Scheduler_simple_Ready_queue_enqueue_first(
  Thread_Control    *the_thread
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/schedulersimple.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
