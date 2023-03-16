/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerPriority
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerPriority which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 *  Copryight (c) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_H
#define _RTEMS_SCORE_SCHEDULERPRIORITY_H

#include <rtems/score/chain.h>
#include <rtems/score/prioritybitmap.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreSchedulerPriority Deterministic Priority Scheduler
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This group contains the Deterministic Priority Scheduler
 *   implementation.
 *
 * @{
 */

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
    _Scheduler_priority_Update_priority,  /* update priority entry point */ \
    _Scheduler_default_Map_priority,      /* map priority entry point */ \
    _Scheduler_default_Unmap_priority,    /* unmap priority entry point */ \
    SCHEDULER_DEFAULT_SMP_OPERATIONS \
    _Scheduler_priority_Node_initialize,  /* node initialize entry point */ \
    _Scheduler_default_Node_destroy,      /* node destroy entry point */ \
    _Scheduler_default_Release_job,       /* new period of task */ \
    _Scheduler_default_Cancel_job,        /* cancel period of task */ \
    _Scheduler_default_Start_idle         /* start idle entry point */ \
    SCHEDULER_DEFAULT_SET_AFFINITY_OPERATION \
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
  Chain_Control Ready[ RTEMS_ZERO_LENGTH_ARRAY ];
} Scheduler_priority_Context;

/**
 * @brief Data for ready queue operations.
 */
typedef struct {
  /**
   * @brief The thread priority currently used by the scheduler.
   */
  unsigned int current_priority;

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
 *
 * This routine initializes the priority scheduler.
 *
 * @param scheduler The scheduler to initialize.
 */
void _Scheduler_priority_Initialize( const Scheduler_Control *scheduler );

/**
 * @brief Blocks the thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread to block.
 * @param[in, out] node The @a thread's scheduler node.
 */
void _Scheduler_priority_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Sets the heir thread to be the next ready thread.
 *
 * This kernel routine sets the heir thread to be the next ready thread
 * by invoking the_scheduler->ready_queue->operations->first().
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread for the operation.
 */
void _Scheduler_priority_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 * @brief Unblocks the thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread to unblock.
 * @param[in, out] node The @a thread's scheduler node.
 */
void _Scheduler_priority_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread for the operation.
 * @param base_node The thread's scheduler node.
 */
void _Scheduler_priority_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *base_node
);

/**
 * @brief Initializes the node with the given priority.
 *
 * @param scheduler The scheduler instance.
 * @param[out] node The node to initialize.
 * @param the_thread The thread of the scheduler node.
 * @param priority The priority for the initialization.
 */
void _Scheduler_priority_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Performs the yield of a thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread that performed the yield operation.
 * @param node The scheduler node of @a the_thread.
 */
void _Scheduler_priority_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
