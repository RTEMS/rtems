/**
 *  @file  rtems/score/schedulerpriority.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of threads for the priority-based scheduler.
 */

/*
 *  Copryight (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_H
#define _RTEMS_SCORE_SCHEDULERPRIORITY_H

/**
 *  @addtogroup ScoreScheduler
 *
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/percpu.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/wkspace.h>

/**
 * This routine initializes the priority scheduler.
 */
void _Scheduler_priority_Initialize(
    Scheduler_Control    *the_scheduler
);

/**
 *  This routine removes @a the_thread from the scheduling decision, 
 *  that is, removes it from the ready queue.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */
void _Scheduler_priority_Block( 
    Scheduler_Control *the_scheduler,
    Thread_Control    *the_thread 
);

/**
 *  This kernel routine sets the heir thread to be the next ready thread 
 *  by invoking the_scheduler->ready_queue->operations->first().
 */
void _Scheduler_priority_Schedule(
    Scheduler_Control *the_scheduler
);

/**
 * This routine allocates @a the_thread->scheduler.
 */
void * _Scheduler_priority_Thread_scheduler_allocate(
    Scheduler_Control   *the_scheduler,
    Thread_Control      *the_thread
);

/**
 * This routine frees @a the_thread->scheduler.
 */
void _Scheduler_priority_Thread_scheduler_free(
    Scheduler_Control   *the_scheduler,
    Thread_Control      *the_thread
);

/**
 * This routine updates @a the_thread->scheduler based on @a the_scheduler 
 * structures and thread state
 */
void _Scheduler_priority_Thread_scheduler_update(
    Scheduler_Control   *the_scheduler,
    Thread_Control      *the_thread
);

/**
 *  This routine adds @a the_thread to the scheduling decision, 
 *  that is, adds it to the ready queue and 
 *  updates any appropriate scheduling variables, for example the heir thread.
 */
void _Scheduler_priority_Unblock(
    Scheduler_Control *the_scheduler,
    Thread_Control    *the_thread 
);

/**
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread in the queue.
 */
void _Scheduler_priority_Yield(
    Scheduler_Control *the_scheduler
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/schedulerpriority.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
