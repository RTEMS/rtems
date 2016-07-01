/**
 *  @file  rtems/score/scheduleredf.h
 *
 *  @brief Data Related to the Manipulation of Threads for the EDF Scheduler
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of threads for the EDF scheduler.
 */

/*
 *  Copryight (c) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULEREDF_H
#define _RTEMS_SCORE_SCHEDULEREDF_H

#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/rbtree.h>

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreSchedulerEDF EDF Scheduler
 *
 *  @ingroup ScoreScheduler
 */
/**@{*/

/*
 * Actually the EDF scheduler supports a maximum priority of
 * 0x7fffffffffffffff, but the user API is limited to uint32_t or int for
 * thread priorities.  Ignore ILP64 targets for now.
 */
#define SCHEDULER_EDF_MAXIMUM_PRIORITY INT_MAX

/**
 *  Entry points for the Earliest Deadline First Scheduler.
 */
#define SCHEDULER_EDF_ENTRY_POINTS \
  { \
    _Scheduler_EDF_Initialize,       /* initialize entry point */ \
    _Scheduler_EDF_Schedule,         /* schedule entry point */ \
    _Scheduler_EDF_Yield,            /* yield entry point */ \
    _Scheduler_EDF_Block,            /* block entry point */ \
    _Scheduler_EDF_Unblock,          /* unblock entry point */ \
    _Scheduler_EDF_Update_priority,  /* update priority entry point */ \
    _Scheduler_EDF_Map_priority,     /* map priority entry point */ \
    _Scheduler_EDF_Unmap_priority,   /* unmap priority entry point */ \
    SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_EDF_Node_initialize,  /* node initialize entry point */ \
    _Scheduler_default_Node_destroy, /* node destroy entry point */ \
    _Scheduler_EDF_Release_job,      /* new period of task */ \
    _Scheduler_default_Tick,         /* tick entry point */ \
    _Scheduler_default_Start_idle    /* start idle entry point */ \
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
  }

typedef struct {
  /**
   * @brief Basic scheduler context.
   */
  Scheduler_Context Base;

  /**
   * Top of the ready queue.
   */
  RBTree_Control Ready;
} Scheduler_EDF_Context;

/**
 * @brief Scheduler node specialization for EDF schedulers.
 */
typedef struct {
  /**
   * @brief Basic scheduler node.
   */
  Scheduler_Node Base;

  /**
   * Pointer to corresponding Thread Control Block.
   */
  Thread_Control *thread;
  /**
   * Rbtree node related to this thread.
   */
  RBTree_Node Node;

  /**
   * @brief The thread priority used by this scheduler instance in case no job
   * is released.
   */
  Priority_Control background_priority;

  /**
   * @brief The thread priority currently used by this scheduler instance.
   */
  Priority_Control current_priority;
} Scheduler_EDF_Node;

/**
 *  @brief Initialize EDF scheduler.
 *
 *  This routine initializes the EDF scheduler.
 *
 *  @param[in] scheduler The scheduler instance.
 */
void _Scheduler_EDF_Initialize( const Scheduler_Control *scheduler );

/**
 *  @brief Removes thread from ready queue.
 *
 *  This routine removes @a the_thread from the scheduling decision,
 *  that is, removes it from the ready queue.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread is the thread to be blocked.
 */
void _Scheduler_EDF_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Sets the heir thread to be the next ready thread
 *  in the rbtree ready queue.
 *
 *  This kernel routine sets the heir thread to be the next ready thread
 *  in the rbtree ready queue.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread being scheduled.
 */
void _Scheduler_EDF_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Initializes an EDF specific scheduler node of @a the_thread.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] node being initialized.
 *  @param[in] the_thread the thread of the node.
 *  @param[in] priority The thread priority.
 */
void _Scheduler_EDF_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 *  @brief Adds @a the_thread to the scheduling decision.
 *
 *  This routine adds @a the_thread to the scheduling decision, that is,
 *  adds it to the ready queue and updates any appropriate scheduling
 *  variables, for example the heir thread.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread will be unblocked.
 */
Scheduler_Void_or_thread _Scheduler_EDF_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

Scheduler_Void_or_thread _Scheduler_EDF_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

Priority_Control _Scheduler_EDF_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
);

Priority_Control _Scheduler_EDF_Unmap_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
);

/**
 *  @brief invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread
 *  with equal deadline.
 *
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread in the queue with
 *  equal deadline. This does not have to happen very often.
 *
 *  This routine will remove the specified THREAD from the ready queue
 *  and place it back. The rbtree ready queue is responsible for FIFO ordering
 *  in such a case.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in,out] the_thread The yielding thread.
 */
Scheduler_Void_or_thread _Scheduler_EDF_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Called when a new job of task is released.
 *
 *  This routine is called when a new job of task is released.
 *  It is called only from Rate Monotonic manager in the beginning
 *  of new period.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread is the owner of the job.
 *  @param[in] deadline of the new job from now. If equal to 0,
 *             the job was cancelled or deleted, thus a running task
 *             has to be suspended.
 */
void _Scheduler_EDF_Release_job (
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  uint64_t                 deadline
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
