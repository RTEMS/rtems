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
    _Scheduler_EDF_Cancel_job,       /* cancel period of task */ \
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
   * Rbtree node related to this thread.
   */
  RBTree_Node Node;

  /**
   * @brief The thread priority currently used for this scheduler instance.
   */
  Priority_Control priority;
} Scheduler_EDF_Node;

/**
 *  @brief Initialize EDF scheduler.
 *
 *  This routine initializes the EDF scheduler.
 *
 *  @param[in] scheduler The scheduler instance.
 */
void _Scheduler_EDF_Initialize( const Scheduler_Control *scheduler );

void _Scheduler_EDF_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
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

void _Scheduler_EDF_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_EDF_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

Priority_Control _Scheduler_EDF_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
);

Priority_Control _Scheduler_EDF_Unmap_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
);

void _Scheduler_EDF_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_EDF_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  uint64_t                 deadline,
  Thread_queue_Context    *queue_context
);

void _Scheduler_EDF_Cancel_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  Thread_queue_Context    *queue_context
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
