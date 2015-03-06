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

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreSchedulerEDF EDF Scheduler
 *
 *  @ingroup ScoreScheduler
 */
/**@{*/

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
    _Scheduler_EDF_Change_priority,  /* change priority entry point */ \
    SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_EDF_Node_initialize,  /* node initialize entry point */ \
    _Scheduler_default_Node_destroy, /* node destroy entry point */ \
    _Scheduler_EDF_Update_priority,  /* update priority entry point */ \
    _Scheduler_EDF_Priority_compare, /* compares two priorities */ \
    _Scheduler_EDF_Release_job,      /* new period of task */ \
    _Scheduler_default_Tick,         /* tick entry point */ \
    _Scheduler_default_Start_idle    /* start idle entry point */ \
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
  }

/**
 * This is just a most significant bit of Priority_Control type. It
 * distinguishes threads which are deadline driven (priority
 * represented by a lower number than @a SCHEDULER_EDF_PRIO_MSB) from those
 * ones who do not have any deadlines and thus are considered background
 * tasks.
 */
#define SCHEDULER_EDF_PRIO_MSB 0x80000000

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
 * @typedef Scheduler_EDF_Queue_state
 *
 * This enumeration distiguishes state of a thread with respect to the
 * ready queue.
 */
typedef enum {
  SCHEDULER_EDF_QUEUE_STATE_NOT_PRESENTLY,
  SCHEDULER_EDF_QUEUE_STATE_YES,
  SCHEDULER_EDF_QUEUE_STATE_NEVER_HAS_BEEN
} Scheduler_EDF_Queue_state;

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
   * State of the thread with respect to ready queue.
   */
  Scheduler_EDF_Queue_state queue_state;
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
 *  @param[in] the_thread being initialized.
 */
void _Scheduler_EDF_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Updates position in the ready queue of @a the_thread.
 *
 *  This routine updates position in the ready queue of @a the_thread.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread will have its scheduler specific information
 *             structure updated.
 *  @param[in] new_priority is the desired new priority.
 */
void _Scheduler_EDF_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority
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

Scheduler_Void_or_thread _Scheduler_EDF_Change_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority,
  bool                     prepend_it
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
 *  @brief Explicitly compare absolute dedlines (priorities) of threads.
 *
 * This routine explicitly compares absolute dedlines (priorities) of threads.
 * In case of EDF scheduling time overflow is taken into account.
 *
 * @retval >0 for p1 > p2; 0 for p1 == p2; <0 for p1 < p2.
 */
int _Scheduler_EDF_Priority_compare (
  Priority_Control p1,
  Priority_Control p2
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
  uint32_t                 deadline
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
