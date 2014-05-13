/**
 *  @file  rtems/score/schedulerpriority.h
 *
 *  @brief Thread Manipulation with the Priority-Based Scheduler
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_H
#define _RTEMS_SCORE_SCHEDULERPRIORITY_H

#include <rtems/score/chain.h>
#include <rtems/score/prioritybitmap.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreSchedulerDPS Deterministic Priority Scheduler
 *
 * @ingroup ScoreScheduler
 */
/**@{*/

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__) && defined(RTEMS_SMP)
  #define SCHEDULER_PRIORITY_ADDITIONAL_SMP_ENTRY_POINTS \
    _Scheduler_default_Get_affinity,     /* get affinity entry point */ \
    _Scheduler_default_Set_affinity      /* set affinity entry point */
#else
  #define SCHEDULER_PRIORITY_ADDITIONAL_SMP_ENTRY_POINTS
#endif

/**
 *  Entry points for the Deterministic Priority Based Scheduler.
 */
#define SCHEDULER_PRIORITY_ENTRY_POINTS \
  { \
    _Scheduler_priority_Initialize,       /* initialize entry point */ \
    _Scheduler_priority_Schedule,         /* schedule entry point */ \
    _Scheduler_priority_Yield,            /* yield entry point */ \
    _Scheduler_priority_Block,            /* block entry point */ \
    _Scheduler_priority_Unblock,          /* unblock entry point */ \
    _Scheduler_default_Allocate,         /* allocate entry point */ \
    _Scheduler_default_Free,             /* free entry point */ \
    _Scheduler_priority_Update,           /* update entry point */ \
    _Scheduler_priority_Enqueue,          /* enqueue entry point */ \
    _Scheduler_priority_Enqueue_first,    /* enqueue_first entry point */ \
    _Scheduler_priority_Extract,          /* extract entry point */ \
    _Scheduler_priority_Priority_compare, /* compares two priorities */ \
    _Scheduler_default_Release_job,       /* new period of task */ \
    _Scheduler_default_Tick,              /* tick entry point */ \
    _Scheduler_default_Start_idle,        /* start idle entry point */ \
    SCHEDULER_PRIORITY_ADDITIONAL_SMP_ENTRY_POINTS \
  }

typedef struct {
  /**
   * @brief Basic scheduler context.
   */
  Scheduler_Context Base;

  /**
   * @brief Bit map to indicate non-empty ready queues.
   */
  Priority_bit_map_Control Bit_map;

  /**
   * @brief One ready queue per priority level.
   */
  Chain_Control Ready[ 0 ];
} Scheduler_priority_Context;

/**
 * @brief Data for ready queue operations.
 */
typedef struct {
  /** This field points to the Ready FIFO for this thread's priority. */
  Chain_Control                        *ready_chain;

  /** This field contains precalculated priority map indices. */
  Priority_bit_map_Information          Priority_map;
} Scheduler_priority_Ready_queue;

/**
 * @brief Scheduler node specialization for Deterministic Priority schedulers.
 */
typedef struct {
  /**
   * @brief Basic scheduler node.
   */
  Scheduler_Node Base;

  /**
   * @brief The associated ready queue of this node.
   */
  Scheduler_priority_Ready_queue Ready_queue;
} Scheduler_priority_Node;

/**
 * @brief Initializes the priority scheduler.
 * This routine initializes the priority scheduler.
 */
void _Scheduler_priority_Initialize( const Scheduler_Control *scheduler );

/**
 *  @brief Removes @a the_thread from the scheduling decision.
 *
 *  This routine removes @a the_thread from the scheduling decision,
 *  that is, removes it from the ready queue.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  @param[in] the_thread is the thread to be blocked
 */
void _Scheduler_priority_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Sets the heir thread to be the next ready thread.
 *
 *  This kernel routine sets the heir thread to be the next ready thread
 *  by invoking the_scheduler->ready_queue->operations->first().
 */
void _Scheduler_priority_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Update the scheduler priority.
 *  This routine updates @a the_thread->scheduler based on @a the_scheduler
 *  structures and thread state.
 *
 *  @param[in] the_thread will have its scheduler specific information
 *             structure updated.
 */
void _Scheduler_priority_Update(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Add @a the_thread to the scheduling decision.
 *
 *  This routine adds @a the_thread to the scheduling decision,
 *  that is, adds it to the ready queue and
 *  updates any appropriate scheduling variables, for example the heir thread.
 *
 *  @param[in] the_thread will be unblocked
 */
void _Scheduler_priority_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief The specified THREAD yields.
 *
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread in the queue.
 *
 *  This routine will remove the specified THREAD from the ready queue
 *  and place it immediately at the rear of this chain.  Reset timeslice
 *  and yield the processor functions both use this routine, therefore if
 *  reset is true and this is the only thread on the queue then the
 *  timeslice counter is reset.  The heir THREAD will be updated if the
 *  running is also the currently the heir.
 *
 *  - INTERRUPT LATENCY:
 *    + ready chain
 *    + select heir
 *
 *  @param[in,out] thread The yielding thread.
 */
void _Scheduler_priority_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Puts @a the_thread on to the priority-based ready queue.
 *
 *  This routine puts @a the_thread on to the priority-based ready queue.
 *
 *  @param[in] the_thread will be enqueued at the TAIL of its priority.
 */
void _Scheduler_priority_Enqueue(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Puts @a the_thread to the head of the ready queue.
 *
 *  This routine puts @a the_thread to the head of the ready queue.
 *  For priority-based ready queues, the thread will be the first thread
 *  at its priority level.
 *
 *  @param[in] the_thread will be enqueued at the HEAD of its priority.
 */
void _Scheduler_priority_Enqueue_first(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Remove a specific thread from scheduler.
 *
 *  This routine removes a specific thread from the scheduler's set
 *  of ready threads.
 *
 *  @param[in] the_thread will be extracted from the ready set.
 */
void _Scheduler_priority_Extract(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Compare two priorities.
 *
 *  This routine compares two priorities.
 */
int _Scheduler_priority_Priority_compare(
  Priority_Control   p1,
  Priority_Control   p2
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
