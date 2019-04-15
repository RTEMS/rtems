/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerSMPSimple
 *
 * @brief Simple SMP Scheduler API
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2013, 2018 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_SMP_H
#define _RTEMS_SCORE_SCHEDULERSIMPLE_SMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/schedulersmp.h>

/**
 * @defgroup RTEMSScoreSchedulerSMPSimple Simple Priority SMP Scheduler
 *
 * @ingroup RTEMSScoreSchedulerSMP
 *
 * @brief Simple Priority SMP Scheduler
 *
 * The Simple Priority SMP Scheduler allocates a processor for the processor
 * count highest priority ready threads.  The thread priority and position in
 * the ready chain are the only information to determine the scheduling
 * decision.  Threads with an allocated processor are in the scheduled chain.
 * After initialization the scheduled chain has exactly processor count nodes.
 * Each processor has exactly one allocated thread after initialization.  All
 * enqueue and extract operations may exchange threads with the scheduled
 * chain.  One thread will be added and another will be removed.  The scheduled
 * and ready chain is ordered according to the thread priority order.  The
 * chain insert operations are O(count of ready threads), thus this scheduler
 * is unsuitable for most real-time applications.
 *
 * The thread preempt mode will be ignored.
 *
 * @{
 */

typedef struct {
  Scheduler_SMP_Context Base;
  Chain_Control         Ready;
} Scheduler_simple_SMP_Context;

#define SCHEDULER_SIMPLE_SMP_MAXIMUM_PRIORITY 255

/**
 * @brief Entry points for the Simple SMP Scheduler.
 */
#define SCHEDULER_SIMPLE_SMP_ENTRY_POINTS \
  { \
    _Scheduler_simple_SMP_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_simple_SMP_Yield, \
    _Scheduler_simple_SMP_Block, \
    _Scheduler_simple_SMP_Unblock, \
    _Scheduler_simple_SMP_Update_priority, \
    _Scheduler_default_Map_priority, \
    _Scheduler_default_Unmap_priority, \
    _Scheduler_simple_SMP_Ask_for_help, \
    _Scheduler_simple_SMP_Reconsider_help_request, \
    _Scheduler_simple_SMP_Withdraw_node, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_simple_SMP_Add_processor, \
    _Scheduler_simple_SMP_Remove_processor, \
    _Scheduler_simple_SMP_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Cancel_job, \
    _Scheduler_default_Tick, \
    _Scheduler_SMP_Start_idle \
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
  }

/**
 * @brief Initializes the scheduler's context.
 *
 * @param scheduler The scheduler instance to initialize the context of.
 */
void _Scheduler_simple_SMP_Initialize( const Scheduler_Control *scheduler );

/**
 * @brief Initializes the node with the given attributes.
 *
 * @param scheduler The scheduler instance.
 * @param[out] node The node to initialize.
 * @param the_thread The user of the node, if RTEMS_SMP is defined.
 * @param priority The priority of the node to initialize.
 */
void _Scheduler_simple_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Blocks a thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] thread The thread to block.
 * @param[in, out] node The scheduler node of @a thread.
 */
void _Scheduler_simple_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Unblocks a thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] thread The thread to unblock.
 * @param[in, out] node The scheduler node of @a thread.
 */
void _Scheduler_simple_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread of @a node.
 * @param node The node to update the priority of.
 */
void _Scheduler_simple_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Asks for help.
 *
 * @param scheduler The scheduler instance to ask for help.
 * @param the_thread The thread needing help.
 * @param node The scheduler node.
 *
 * @retval true Ask for help was successful.
 * @retval false Ask for help was not successful.
 */
bool _Scheduler_simple_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Reconsiders help.
 *
 * @param scheduler The scheduler instance to reconsider the help
 *   request.
 * @param the_thread The thread reconsidering a help request.
 * @param node The scheduler node.
 */
void _Scheduler_simple_SMP_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Withdraws node.
 *
 * @param scheduler The scheduler instance to withdraw the node.
 * @param the_thread The thread using the node.
 * @param node The scheduler node to withdraw.
 * @param next_state The next thread scheduler state in the case the node is
 *   scheduled.
 */
void _Scheduler_simple_SMP_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
);

/**
 * @brief Adds @a idle to @a scheduler.
 *
 * @param[in, out] scheduler The scheduler instance to add the processor to.
 * @param idle The idle thread control.
 */
void _Scheduler_simple_SMP_Add_processor(
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
Thread_Control *_Scheduler_simple_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  struct Per_CPU_Control  *cpu
);

/**
 * Performs a yield operation for the thread.
 *
 * @param scheduler The scheduler instance.
 * @param thread The thread to yield.
 * @param[in, out] node The node of the thread to perform a yield.
 */
void _Scheduler_simple_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
