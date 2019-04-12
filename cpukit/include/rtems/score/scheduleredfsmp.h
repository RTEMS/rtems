/**
 * @file
 *
 * @brief EDF SMP Scheduler API
 *
 * @ingroup RTEMSScoreSchedulerSMPEDF
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
 * @defgroup RTEMSScoreSchedulerSMPEDF EDF Priority SMP Scheduler
 *
 * @ingroup RTEMSScoreSchedulerSMP
 *
 * @brief EDF Priority SMP Scheduler
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

/**
 * @brief Initializes the context of the scheduler control.
 *
 * @param scheduler The scheduler control.
 */
void _Scheduler_EDF_SMP_Initialize( const Scheduler_Control *scheduler );

/**
 * @brief Initializes the node with the given priority.
 *
 * @param scheduler The scheduler instance.
 * @param[out] node The node to initialize.
 * @param the_thread The thread of the scheduler node.
 * @param priority The priority for the initialization.
 */
void _Scheduler_EDF_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Blocks the thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread to block.
 * @param[in, out] node The @a thread's scheduler node.
 */
void _Scheduler_EDF_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Unblocks the thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread to unblock.
 * @param[in, out] node The @a thread's scheduler node.
 */
void _Scheduler_EDF_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread for the operation.
 * @param node The thread's scheduler node.
 */
void _Scheduler_EDF_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Asks for help operation.
 *
 * @param scheduler The scheduler instance to ask for help.
 * @param the_thread The thread needing help.
 * @param node The scheduler node.
 *
 * @retval true Ask for help was successful.
 * @retval false Ask for help was not successful.
 */
bool _Scheduler_EDF_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Reconsiders help operation.
 *
 * @param scheduler The scheduler instance to reconsider the help
 *   request.
 * @param the_thread The thread reconsidering a help request.
 * @param node The scheduler node.
 */
void _Scheduler_EDF_SMP_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Withdraws node operation.
 *
 * @param scheduler The scheduler instance to withdraw the node.
 * @param the_thread The thread using the node.
 * @param node The scheduler node to withdraw.
 * @param next_state The next thread scheduler state in case the node is
 *   scheduled.
 */
void _Scheduler_EDF_SMP_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
);

/**
 * @brief Pin thread operation.
 *
 * @param scheduler The scheduler instance of the specified processor.
 * @param the_thread The thread to pin.
 * @param node The scheduler node of the thread.
 * @param cpu The processor to pin the thread.
 */
void _Scheduler_EDF_SMP_Pin(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  struct Per_CPU_Control  *cpu
);

/**
 * @brief Unpin thread operation.
 *
 * @param scheduler The scheduler instance of the specified processor.
 * @param the_thread The thread to unpin.
 * @param node The scheduler node of the thread.
 * @param cpu The processor to unpin the thread.
 */
void _Scheduler_EDF_SMP_Unpin(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  struct Per_CPU_Control  *cpu
);

/**
 * @brief Adds processor.
 *
 * @param[in, out] scheduler The scheduler instance to add the processor to.
 * @param idle The idle thread of the processor to add.
 */
void _Scheduler_EDF_SMP_Add_processor(
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
Thread_Control *_Scheduler_EDF_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  struct Per_CPU_Control  *cpu
);

/**
 * @brief Performs the yield of a thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread The thread that performed the yield operation.
 * @param node The scheduler node of @a the_thread.
 */
void _Scheduler_EDF_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Starts an idle thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] the_thread An idle thread.
 * @param cpu The cpu for the operation.
 */
void _Scheduler_EDF_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  struct Per_CPU_Control  *cpu
);

/**
 * @brief Checks if the processor set of the scheduler is the subset of the affinity set.
 *
 * Default implementation of the set affinity scheduler operation.
 *
 * @param scheduler This parameter is unused.
 * @param thread This parameter is unused.
 * @param node This parameter is unused.
 * @param affinity The new processor affinity set for the thread.
 *
 * @retval true The processor set of the scheduler is a subset of the affinity set.
 * @retval false The processor set of the scheduler is not a subset of the affinity set.
 */
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
