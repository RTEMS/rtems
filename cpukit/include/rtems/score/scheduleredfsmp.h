/**
 * @file
 *
 * @brief EDF SMP Scheduler API
 *
 * @ingroup ScoreSchedulerSMPEDF
 */

/*
 * Copyright (c) 2017, 2018 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULEREDFSMP_H
#define _RTEMS_SCORE_SCHEDULEREDFSMP_H

#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>
#include <rtems/score/schedulersmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreSchedulerSMPEDF EDF Priority SMP Scheduler
 *
 * @ingroup ScoreSchedulerSMP
 *
 * @{
 */

typedef struct {
  Scheduler_SMP_Node Base;

  /**
   * @brief Generation number to ensure FIFO/LIFO order for threads of the same
   * priority across different ready queues.
   */
  int64_t generation;

  /**
   * @brief The ready queue index depending on the processor affinity and
   * pinning of the thread.
   *
   * The ready queue index zero is used for threads with a one-to-all thread
   * processor affinity.  Threads with a one-to-one processor affinity use the
   * processor index plus one as the ready queue index.
   */
  uint8_t ready_queue_index;

  /**
   * @brief Ready queue index according to thread affinity.
   */
  uint8_t affinity_ready_queue_index;

  /**
   * @brief Ready queue index according to thread pinning.
   */
  uint8_t pinning_ready_queue_index;
} Scheduler_EDF_SMP_Node;

typedef struct {
  /**
   * @brief Chain node for Scheduler_SMP_Context::Affine_queues.
   */
  Chain_Node Node;

  /**
   * @brief The ready threads of the corresponding affinity.
   */
  RBTree_Control Queue;

  /**
   * @brief The scheduled thread of the corresponding processor.
   */
  Scheduler_EDF_SMP_Node *scheduled;
} Scheduler_EDF_SMP_Ready_queue;

typedef struct {
  Scheduler_SMP_Context Base;

  /**
   * @brief Current generation for LIFO (index 0) and FIFO (index 1) ordering.
   */
  int64_t generations[ 2 ];

  /**
   * @brief Chain of ready queues with affine threads to determine the highest
   * priority ready thread.
   */
  Chain_Control Affine_queues;

  /**
   * @brief A table with ready queues.
   *
   * The index zero queue is used for threads with a one-to-all processor
   * affinity.  Index one corresponds to processor index zero, and so on.
   */
  Scheduler_EDF_SMP_Ready_queue Ready[ RTEMS_ZERO_LENGTH_ARRAY ];
} Scheduler_EDF_SMP_Context;

#define SCHEDULER_EDF_SMP_ENTRY_POINTS \
  { \
    _Scheduler_EDF_SMP_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_EDF_SMP_Yield, \
    _Scheduler_EDF_SMP_Block, \
    _Scheduler_EDF_SMP_Unblock, \
    _Scheduler_EDF_SMP_Update_priority, \
    _Scheduler_EDF_Map_priority, \
    _Scheduler_EDF_Unmap_priority, \
    _Scheduler_EDF_SMP_Ask_for_help, \
    _Scheduler_EDF_SMP_Reconsider_help_request, \
    _Scheduler_EDF_SMP_Withdraw_node, \
    _Scheduler_EDF_SMP_Pin, \
    _Scheduler_EDF_SMP_Unpin, \
    _Scheduler_EDF_SMP_Add_processor, \
    _Scheduler_EDF_SMP_Remove_processor, \
    _Scheduler_EDF_SMP_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_EDF_Release_job, \
    _Scheduler_EDF_Cancel_job, \
    _Scheduler_default_Tick, \
    _Scheduler_EDF_SMP_Start_idle, \
    _Scheduler_EDF_SMP_Set_affinity \
  }

void _Scheduler_EDF_SMP_Initialize( const Scheduler_Control *scheduler );

void _Scheduler_EDF_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

void _Scheduler_EDF_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

void _Scheduler_EDF_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

void _Scheduler_EDF_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

bool _Scheduler_EDF_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_EDF_SMP_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_EDF_SMP_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
);

void _Scheduler_EDF_SMP_Pin(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  struct Per_CPU_Control  *cpu
);

void _Scheduler_EDF_SMP_Unpin(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  struct Per_CPU_Control  *cpu
);

void _Scheduler_EDF_SMP_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
);

Thread_Control *_Scheduler_EDF_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  struct Per_CPU_Control  *cpu
);

void _Scheduler_EDF_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

void _Scheduler_EDF_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  struct Per_CPU_Control  *cpu
);

bool _Scheduler_EDF_SMP_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node,
  const Processor_mask    *affinity
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_SCHEDULEREDFSMP_H */
