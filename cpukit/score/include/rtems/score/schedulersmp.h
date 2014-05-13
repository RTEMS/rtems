/**
 * @file
 *
 * @brief SMP Scheduler API
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSMP_H
#define _RTEMS_SCORE_SCHEDULERSMP_H

#include <rtems/score/chain.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSchedulerSMP SMP Scheduler
 *
 * @ingroup ScoreScheduler
 *
 * @{
 */

/**
 * @brief Scheduler context specialization for SMP schedulers.
 */
typedef struct {
  /**
   * @brief Basic scheduler context.
   */
  Scheduler_Context Base;

  /**
   * @brief The chain of scheduled nodes.
   */
  Chain_Control Scheduled;
} Scheduler_SMP_Context;

/**
 * @brief SMP scheduler node states.
 */
typedef enum {
  /**
   * @brief This scheduler node is blocked.
   *
   * A scheduler node is blocked if the corresponding thread is not ready.
   */
  SCHEDULER_SMP_NODE_BLOCKED,

  /**
   * @brief The scheduler node is scheduled.
   *
   * A scheduler node is scheduled if the corresponding thread is ready and the
   * scheduler allocated a processor for it.  A scheduled node is assigned to
   * exactly one processor.  The sum of scheduled and in the air nodes equals
   * the processor count owned by a scheduler instance.
   */
  SCHEDULER_SMP_NODE_SCHEDULED,

  /**
   * @brief This scheduler node is ready.
   *
   * A scheduler node is ready if the corresponding thread is ready and the
   * scheduler did not allocate a processor for it.
   */
  SCHEDULER_SMP_NODE_READY,

  /**
   * @brief This scheduler node is in the air.
   *
   * A scheduled node is in the air if it has an allocated processor and the
   * corresponding thread is in a transient state.  Such a node is not an
   * element of the set of scheduled nodes.  The extract operation on a
   * scheduled node will produce a scheduler node in the air (see also
   * _Thread_Set_transient()).  The next enqueue or schedule operation will
   * decide what to do based on this state indication.  It can either place the
   * scheduler node back on the set of scheduled nodes and the thread can keep
   * its allocated processor, or it can take the processor away from the thread
   * and give the processor to another thread of higher priority.
   */
  SCHEDULER_SMP_NODE_IN_THE_AIR
} Scheduler_SMP_Node_state;

/**
 * @brief Scheduler node specialization for SMP schedulers.
 */
typedef struct {
  /**
   * @brief Basic scheduler node.
   */
  Scheduler_Node Base;

  /**
   * @brief The state of this node.
   */
  Scheduler_SMP_Node_state state;
} Scheduler_SMP_Node;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMP_H */
