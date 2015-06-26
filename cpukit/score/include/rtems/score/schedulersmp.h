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

  /**
   * @brief Chain of the available idle threads.
   *
   * Idle threads are used for the scheduler helping protocol.  It is crucial
   * that the idle threads preserve their relative order.  This is the case for
   * this priority based scheduler.
   */
  Chain_Control Idle_threads;
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
   * exactly one processor.  The count of scheduled nodes in this scheduler
   * instance equals the processor count owned by the scheduler instance.
   */
  SCHEDULER_SMP_NODE_SCHEDULED,

  /**
   * @brief This scheduler node is ready.
   *
   * A scheduler node is ready if the corresponding thread is ready and the
   * scheduler did not allocate a processor for it.
   */
  SCHEDULER_SMP_NODE_READY
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

  /**
   * @brief The current priority of thread owning this node.
   */
  Priority_Control priority;
} Scheduler_SMP_Node;

void _Scheduler_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control *thread,
  struct Per_CPU_Control *cpu
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMP_H */
