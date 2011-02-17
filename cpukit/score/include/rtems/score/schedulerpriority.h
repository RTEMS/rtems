/**
 *  @file  rtems/score/schedulerpriority.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of threads for the priority-based scheduler.
 */

/*
 *  Copryight (c) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_H
#define _RTEMS_SCORE_SCHEDULERPRIORITY_H

#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/percpu.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/wkspace.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup ScoreScheduler
 *
 */
/**@{*/

/**
 *  Entry points for Scheduler Priority
 */
#define SCHEDULER_PRIORITY_ENTRY_POINTS \
  { \
    .initialize         = _Scheduler_priority_Initialize, \
    .schedule           = _Scheduler_priority_Schedule, \
    .yield              = _Scheduler_priority_Yield, \
    .block              = _Scheduler_priority_Block, \
    .unblock            = _Scheduler_priority_Unblock, \
    .scheduler_allocate = _Scheduler_priority_Thread_scheduler_allocate, \
    .scheduler_free     = _Scheduler_priority_Thread_scheduler_free, \
    .scheduler_update   = _Scheduler_priority_Thread_scheduler_update \
  }

/**
 * This routine initializes the priority scheduler.
 */
void _Scheduler_priority_Initialize(void);

/**
 *  This routine removes @a the_thread from the scheduling decision, 
 *  that is, removes it from the ready queue.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 */
void _Scheduler_priority_Block( 
  Thread_Control    *the_thread 
);

/**
 *  This kernel routine sets the heir thread to be the next ready thread 
 *  by invoking the_scheduler->ready_queue->operations->first().
 */
void _Scheduler_priority_Schedule(void);

/**
 * This routine allocates @a the_thread->scheduler.
 */
void * _Scheduler_priority_Thread_scheduler_allocate(
  Thread_Control      *the_thread
);

/**
 * This routine frees @a the_thread->scheduler.
 */
void _Scheduler_priority_Thread_scheduler_free(
  Thread_Control      *the_thread
);

/**
 * This routine updates @a the_thread->scheduler based on @a the_scheduler 
 * structures and thread state
 */
void _Scheduler_priority_Thread_scheduler_update(
  Thread_Control      *the_thread
);

/**
 *  This routine adds @a the_thread to the scheduling decision, 
 *  that is, adds it to the ready queue and 
 *  updates any appropriate scheduling variables, for example the heir thread.
 */
void _Scheduler_priority_Unblock(
  Thread_Control    *the_thread 
);

/**
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread in the queue.
 *
 *  This routine will remove the running THREAD from the ready queue
 *  and place it immediately at the rear of this chain.  Reset timeslice
 *  and yield the processor functions both use this routine, therefore if
 *  reset is true and this is the only thread on the queue then the
 *  timeslice counter is reset.  The heir THREAD will be updated if the
 *  running is also the currently the heir.
 */
void _Scheduler_priority_Yield( void );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/schedulerpriority.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
