/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreSchedulerStrongAPA.
 */

/*
 * Copyright (C) 2020 Richi Dubey
 * Copyright (C) 2013, 2018 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_SCHEDULERSTRONGAPA_H
#define _RTEMS_SCORE_SCHEDULERSTRONGAPA_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulersmp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Forward Declaration of Per_CPU_Control */
struct Per_CPU_Control;

/**
 * @defgroup RTEMSScoreSchedulerStrongAPA Strong APA Scheduler
 *
 * @ingroup RTEMSScoreSchedulerSMP
 *
 * @brief This group contains the Strong APA Scheduler implementation.
 *
 * This is an implementation of the Strong APA scheduler defined by
 * Cerqueira et al. in Linux's Processor Affinity API, Refined:
 * Shifting Real-Time Tasks Towards Higher Schedulability.
 *
 * The scheduled and ready nodes are accessed via the
 * Scheduler_strong_APA_Context::Ready which helps in backtracking when a
 * node which is executing on a CPU gets blocked. New node is allocated to
 * the cpu by checking all the executing nodes in the affinity set of the
 * node and the subsequent nodes executing on the processors in its
 * affinity set.
 * @{
 */

/**
 * @brief Scheduler node specialization for Strong APA schedulers.
 */
typedef struct {
  /**
   * @brief SMP scheduler node.
   */
  Scheduler_SMP_Node Base;

  /**
   * @brief Chain node for Scheduler_strong_APA_Context::Ready.
   */
  Chain_Node Ready_node;

  /**
   * @brief CPU that this node would preempt in the backtracking part of
   * _Scheduler_strong_APA_Get_highest_ready and
   * _Scheduler_strong_APA_Do_Enqueue.
   */
  struct Per_CPU_Control *cpu_to_preempt;

  /**
   * @brief The associated affinity set of this node.
   */
  Processor_mask Affinity;
} Scheduler_strong_APA_Node;


/**
 * @brief CPU related variables and a CPU_Control to implement BFS.
 */
typedef struct {
  /**
   * @brief CPU in a queue.
   */
  struct Per_CPU_Control *cpu;

  /**
   * @brief The node that would preempt this CPU.
   */
  Scheduler_Node *preempting_node;

  /**
   * @brief Whether or not this cpu has been added to the queue
   * (visited in BFS).
   */
  bool visited;

  /**
   * @brief The node currently executing on this cpu.
   */
  Scheduler_Node *executing;
} Scheduler_strong_APA_CPU;

/**
 * @brief Scheduler context and node definition for Strong APA scheduler.
 */
typedef struct {
  /**
   * @brief @see Scheduler_SMP_Context.
   */
  Scheduler_SMP_Context Base;

  /**
   * @brief Chain of all the ready and scheduled nodes present in
   * the Strong APA scheduler.
   */
  Chain_Control Ready;

  /**
   * @brief Stores cpu-specific variables.
   */
  Scheduler_strong_APA_CPU CPU[ RTEMS_ZERO_LENGTH_ARRAY ];
} Scheduler_strong_APA_Context;

#define SCHEDULER_STRONG_APA_MAXIMUM_PRIORITY 255

/**
 * @brief Entry points for the Strong APA Scheduler.
 */
#define SCHEDULER_STRONG_APA_ENTRY_POINTS \
  { \
    _Scheduler_strong_APA_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_strong_APA_Yield, \
    _Scheduler_strong_APA_Block, \
    _Scheduler_strong_APA_Unblock, \
    _Scheduler_strong_APA_Update_priority, \
    _Scheduler_default_Map_priority, \
    _Scheduler_default_Unmap_priority, \
    _Scheduler_strong_APA_Ask_for_help, \
    _Scheduler_strong_APA_Reconsider_help_request, \
    _Scheduler_strong_APA_Withdraw_node, \
    _Scheduler_strong_APA_Make_sticky, \
    _Scheduler_strong_APA_Clean_sticky, \
    _Scheduler_default_Pin_or_unpin_not_supported, \
    _Scheduler_default_Pin_or_unpin_not_supported, \
    _Scheduler_strong_APA_Add_processor, \
    _Scheduler_strong_APA_Remove_processor, \
    _Scheduler_strong_APA_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Cancel_job, \
    _Scheduler_strong_APA_Start_idle, \
    _Scheduler_strong_APA_Set_affinity \
  }

/**
 * @brief Initializes the scheduler.
 *
 * @param scheduler The scheduler to initialize.
 */
void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler );

/**
 * @brief Initializes the node with the given priority.
 *
 * @param scheduler The scheduler control instance.
 * @param[out] node The node to initialize.
 * @param the_thread The thread of the node to initialize.
 * @param priority The priority for @a node.
 */
void _Scheduler_strong_APA_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Blocks the thread.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] the_thread The thread to block.
 * @param[in, out] node The node of the thread to block.
 */
void _Scheduler_strong_APA_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Unblocks the thread.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] the_thread The thread to unblock.
 * @param[in, out] node The node of the thread to unblock.
 */
void _Scheduler_strong_APA_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node.
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread for the operation.
 * @param[in, out] node The node to update the priority of.
 */
void _Scheduler_strong_APA_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Asks for help.
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread that asks for help.
 * @param node The node of @a the_thread.
 *
 * @retval true The request for help was successful.
 * @retval false The request for help was not successful.
 */
bool _Scheduler_strong_APA_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Reconsiders help request.
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread to reconsider the help request of.
 * @param[in, out] node The node of @a the_thread
 */
void _Scheduler_strong_APA_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Withdraws the node.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] the_thread The thread to change the state to @a next_state.
 * @param[in, out] node The node to withdraw.
 * @param next_state The next state for @a the_thread.
 */
void _Scheduler_strong_APA_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
);

/**
 * @brief Makes the node sticky.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[in, out] the_thread is the thread owning the node.
 *
 * @param[in, out] node is the scheduler node to make sticky.
 */
void _Scheduler_strong_APA_Make_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Cleans the sticky property from the node.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[in, out] the_thread is the thread owning the node.
 *
 * @param[in, out] node is the scheduler node to clean the sticky property.
 */
void _Scheduler_strong_APA_Clean_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Makes the node sticky.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[in, out] the_thread is the thread owning the node.
 *
 * @param[in, out] node is the scheduler node to make sticky.
 */
void _Scheduler_strong_APA_Make_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Cleans the sticky property from the node.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[in, out] the_thread is the thread owning the node.
 *
 * @param[in, out] node is the scheduler node to clean the sticky property.
 */
void _Scheduler_strong_APA_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Adds the idle thread to a processor.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] The idle thread to add to the processor.
 */
void _Scheduler_strong_APA_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
);

/**
 * @brief Removes an idle thread from the given cpu.
 *
 * @param scheduler The scheduler instance.
 * @param cpu The cpu control to remove from @a scheduler.
 *
 * @return The idle thread of the processor.
 */
Thread_Control *_Scheduler_strong_APA_Remove_processor(
  const Scheduler_Control *scheduler,
  struct Per_CPU_Control  *cpu
);

/**
 * @brief Performs a yield operation.
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread to yield.
 * @param[in, out] node The node of @a the_thread.
 */
void _Scheduler_strong_APA_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Starts an idle thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread An idle thread.
 * @param cpu The cpu for the operation.
 */
void _Scheduler_strong_APA_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  struct Per_CPU_Control  *cpu
);

/**
 * @brief Sets the affinity .
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread to yield.
 * @param[in, out] node The node of @a the_thread.
 */
Status_Control _Scheduler_strong_APA_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSTRONGAPA_H */
