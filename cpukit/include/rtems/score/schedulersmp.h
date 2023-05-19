/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerSMP
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerSMP which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSMP_H
#define _RTEMS_SCORE_SCHEDULERSMP_H

#include <rtems/score/chain.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreSchedulerSMP SMP Scheduler
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This group contains the SMP Scheduler implementation.
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

/**
 * @brief Starts an idle thread on the specified cpu.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] idle The idle thread to schedule.
 * @param[out] cpu The cpu to run the idle thread on.
 */
void _Scheduler_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  struct Per_CPU_Control  *cpu
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMP_H */
