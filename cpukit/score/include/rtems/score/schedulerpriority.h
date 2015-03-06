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
    _Scheduler_priority_Change_priority,  /* change priority entry point */ \
    SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_default_Node_initialize,   /* node initialize entry point */ \
    _Scheduler_default_Node_destroy,      /* node destroy entry point */ \
    _Scheduler_priority_Update_priority,  /* update priority entry point */ \
    _Scheduler_priority_Priority_compare, /* compares two priorities */ \
    _Scheduler_default_Release_job,       /* new period of task */ \
    _Scheduler_default_Tick,              /* tick entry point */ \
    _Scheduler_default_Start_idle         /* start idle entry point */ \
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
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
 *  @param[in] scheduler The scheduler instance.
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
 *  @brief Updates the scheduler node to reflect the new priority of the
 *  thread.
 */
void _Scheduler_priority_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority
);

/**
 *  @brief Add @a the_thread to the scheduling decision.
 *
 *  This routine adds @a the_thread to the scheduling decision,
 *  that is, adds it to the ready queue and
 *  updates any appropriate scheduling variables, for example the heir thread.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread will be unblocked
 */
Scheduler_Void_or_thread _Scheduler_priority_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

Scheduler_Void_or_thread _Scheduler_priority_Change_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority,
  bool                     prepend_it
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
 *  @param[in] scheduler The scheduler instance.
 *  @param[in,out] the_thread The yielding thread.
 */
Scheduler_Void_or_thread _Scheduler_priority_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Compare two priorities.
 *
 *  This routine compares two priorities.
 *
 * @retval >0 for p1 > p2; 0 for p1 == p2; <0 for p1 < p2.
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
