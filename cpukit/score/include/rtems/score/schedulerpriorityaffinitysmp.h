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
#include <rtems/score/cpuset.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSchedulerPriorityAffinitySMP Deterministic Priority Affinity SMP Scheduler
 *
 * @ingroup ScoreScheduler
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
    _Scheduler_priority_SMP_Schedule, \
    _Scheduler_priority_SMP_Yield, \
    _Scheduler_priority_SMP_Block, \
    _Scheduler_priority_SMP_Enqueue_fifo, \
    _Scheduler_priority_affinity_SMP_Allocate, \
    _Scheduler_priority_Free, \
    _Scheduler_priority_SMP_Update, \
    _Scheduler_priority_SMP_Enqueue_fifo, \
    _Scheduler_priority_SMP_Enqueue_lifo, \
    _Scheduler_priority_SMP_Extract, \
    _Scheduler_priority_Priority_compare, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Tick, \
    _Scheduler_priority_SMP_Start_idle, \
    _Scheduler_priority_affinity_SMP_Get_affinity, \
    _Scheduler_priority_affinity_SMP_Set_affinity \
  }

/**
 *  @brief Allocates @a the_thread->scheduler.
 *
 *  This routine allocates @a the_thread->scheduler.
 *
 *  @param[in] scheduler points to the scheduler specific information.
 *  @param[in] the_thread is the thread the scheduler is allocating
 *             management memory for.
 */
void * _Scheduler_priority_affinity_SMP_Allocate(
  Scheduler_Control *scheduler,
  Thread_Control    *the_thread
);

/**
 * @brief Get affinity for the priority affinity smp scheduler.
 *
 * @param[in] scheduler The scheduler of the thread.
 * @param[in] thread The associated thread.
 * @param[in] cpusetsize The size of the cpuset.
 * @param[in,out] cpuset The associated affinity set.
 *
 * @retval 0 Successfully got cpuset
 * @retval -1 The cpusetsize is invalid for the system
 */
bool _Scheduler_priority_affinity_SMP_Get_affinity(
  Scheduler_Control *scheduler,
  Thread_Control    *thread,
  size_t             cpusetsize,
  cpu_set_t         *cpuset
);

/** 
 * @brief Set affinity for the priority affinity smp scheduler.
 *
 * @param[in] scheduler The scheduler of the thread.
 * @param[in] thread The associated thread.
 * @param[in] cpusetsize The size of the cpuset.
 * @param[in] cpuset Affinity new affinity set.
 *
 * @retval 0 Successful
 */
bool _Scheduler_priority_affinity_SMP_Set_affinity(
  Scheduler_Control *scheduler,
  Thread_Control    *thread,
  size_t             cpusetsize,
  cpu_set_t         *cpuset
);

/**
 * This structure handles affinity specific data of a thread.
 *
 * @note The attribute priority_sched_info must remain
 *       the first element in the structure so that the 
 *       Scheduler_priority_XXX methods will continue to 
 *       function.
 */
typedef struct {

  /**
   * Data for the Priority Scheduler.
   */
  Scheduler_priority_Per_thread  Priority_sched_info;

  /**
   * Structure containing affinity set data and size
   */
  CPU_set_Control Affinity;
} Scheduler_priority_affinity_SMP_Per_thread;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYSMP_H */
