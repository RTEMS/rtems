/**
 * @file
 *
 * @ingroup ScoreSchedulerPriorityAffinitySMP
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
 * @defgroup ScoreSchedulerPriorityAffinitySMP Deterministic Priority Affinity SMP Scheduler
 *
 * @ingroup ScoreSchedulerPrioritySMP
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
 *  @brief Initializes per thread scheduler information
 *
 *  This routine allocates @a thread->scheduler.
 *
 *  @param[in] scheduler points to the scheduler specific information.
 *  @param[in] node is the node the scheduler is allocating
 *             management memory for.
 *  @param[in] the_thread the thread of the node.
 *  @param[in] priority is the thread priority.
 */
void _Scheduler_priority_affinity_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

void _Scheduler_priority_affinity_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

void _Scheduler_priority_affinity_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

void _Scheduler_priority_affinity_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

bool _Scheduler_priority_affinity_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_priority_affinity_SMP_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_priority_affinity_SMP_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
);

void _Scheduler_priority_affinity_SMP_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
);

Thread_Control *_Scheduler_priority_affinity_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  struct Per_CPU_Control  *cpu
);

/** 
 * @brief Set affinity for the priority affinity SMP scheduler.
 *
 * @param[in] scheduler The scheduler of the thread.
 * @param[in] thread The associated thread.
 * @param[in] affinity The new affinity set.
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
