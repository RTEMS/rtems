/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerPriorityAffinitySMP
 *
 * @brief Deterministic Priority Affinity SMP Scheduler API
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITYAFFINITYSMP_H
#define _RTEMS_SCORE_SCHEDULERPRIORITYAFFINITYSMP_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/schedulersmp.h>
#include <rtems/score/schedulerprioritysmp.h>

#include <sys/cpuset.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreSchedulerPriorityAffinitySMP Deterministic Priority Affinity SMP Scheduler
 *
 * @ingroup RTEMSScoreSchedulerPrioritySMP
 *
 * @brief Deterministic Priority Affinity SMP Scheduler
 *
 * This is an extension of the Deterministic Priority SMP Scheduler. which
 * is an implementation of the global fixed priority scheduler (G-FP). 
 * It adds thread to core affinity support.
 *
 * @note This is the first iteration of this scheduler. It currently tracks
 *       the requested affinity to exercise the Scheduler Framework but it
 *       does not honor that affinity in assigning threads to cores. This
 *       will be added in a subsequent revision.
 * @{
 */

/**
 * @brief Entry points for the Deterministic Priority Affinity SMP Scheduler.
 */
#define SCHEDULER_PRIORITY_AFFINITY_SMP_ENTRY_POINTS \
  { \
    _Scheduler_priority_SMP_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_priority_SMP_Yield, \
    _Scheduler_priority_affinity_SMP_Block, \
    _Scheduler_priority_affinity_SMP_Unblock, \
    _Scheduler_priority_affinity_SMP_Update_priority, \
    _Scheduler_default_Map_priority, \
    _Scheduler_default_Unmap_priority, \
    _Scheduler_priority_affinity_SMP_Ask_for_help, \
    _Scheduler_priority_affinity_SMP_Reconsider_help_request, \
    _Scheduler_priority_affinity_SMP_Withdraw_node, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_priority_affinity_SMP_Add_processor, \
    _Scheduler_priority_affinity_SMP_Remove_processor, \
    _Scheduler_priority_affinity_SMP_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Cancel_job, \
    _Scheduler_default_Tick, \
    _Scheduler_SMP_Start_idle, \
    _Scheduler_priority_affinity_SMP_Set_affinity \
  }

/**
 * @brief Initializes per thread scheduler information.
 *
 * This routine allocates @a thread->scheduler.
 *
 * @param scheduler Points to the scheduler specific information.
 * @param[in, out] node The node the scheduler is allocating
 *            management memory for.
 * @param the_thread The thread of the node.
 * @param priority The thread priority.
 */
void _Scheduler_priority_affinity_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Blocks a thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] The thread to block.
 * @param[in, out] node The scheduler node of the thread.
 */
void _Scheduler_priority_affinity_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Unblocks a thread.
 *
 * @param scheduler The scheduler instance.
 * @param[in, out] The thread to unblock.
 * @param[in, out] node The scheduler node of the thread.
 */
void _Scheduler_priority_affinity_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread of the node.
 * @param[in, out] The node to update the priority of.
 */
void _Scheduler_priority_affinity_SMP_Update_priority(
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
bool _Scheduler_priority_affinity_SMP_Ask_for_help(
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
void _Scheduler_priority_affinity_SMP_Reconsider_help_request(
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
void _Scheduler_priority_affinity_SMP_Withdraw_node(
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
void _Scheduler_priority_affinity_SMP_Add_processor(
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
Thread_Control *_Scheduler_priority_affinity_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  struct Per_CPU_Control  *cpu
);

/** 
 * @brief Sets affinity for the priority affinity SMP scheduler.
 *
 * @param scheduler The scheduler of the thread.
 * @param[in, out] thread The associated thread.
 * @param[in, out] node The scheduler node.
 * @param affinity The new affinity set.
 *
 * @retval true if successful
 * @retval false if unsuccessful
 */
bool _Scheduler_priority_affinity_SMP_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node,
  const Processor_mask    *affinity
);

/**
 * @brief Scheduler node specialization for Deterministic Priority Affinity SMP
 * schedulers.
 *
 * This is a per thread structure.
 */
typedef struct {
  /**
   * @brief SMP priority scheduler node.
   */
  Scheduler_priority_SMP_Node Base;

  /**
   * @brief The thread processor affinity set.
   */
  Processor_mask Affinity;
} Scheduler_priority_affinity_SMP_Node;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYAFFINITYSMP_H */
