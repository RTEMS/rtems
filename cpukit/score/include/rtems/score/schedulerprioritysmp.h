/**
 * @file
 *
 * @ingroup ScoreSchedulerPrioritySMP
 *
 * @brief Deterministic Priority SMP Scheduler API
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

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITYSMP_H
#define _RTEMS_SCORE_SCHEDULERPRIORITYSMP_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/schedulersmp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSchedulerPrioritySMP Deterministic Priority SMP Scheduler
 *
 * @ingroup ScoreSchedulerSMP
 *
 * This is an implementation of the global fixed priority scheduler (G-FP).  It
 * uses one ready chain per priority to ensure constant time insert operations.
 * The scheduled chain uses linear insert operations and has at most processor
 * count entries.  Since the processor and priority count are constants all
 * scheduler operations complete in a bounded execution time.
 *
 * The thread preempt mode will be ignored.
 *
 * @{
 */

/**
 * @brief Scheduler context specialization for Deterministic Priority SMP
 * schedulers.
 */
typedef struct {
  Scheduler_SMP_Context    Base;
  Priority_bit_map_Control Bit_map;
  Chain_Control            Ready[ RTEMS_ZERO_LENGTH_ARRAY ];
} Scheduler_priority_SMP_Context;

/**
 * @brief Scheduler node specialization for Deterministic Priority SMP
 * schedulers.
 */
typedef struct {
  /**
   * @brief SMP scheduler node.
   */
  Scheduler_SMP_Node Base;

  /**
   * @brief The associated ready queue of this node.
   */
  Scheduler_priority_Ready_queue Ready_queue;
} Scheduler_priority_SMP_Node;

/**
 * @brief Entry points for the Priority SMP Scheduler.
 */
#define SCHEDULER_PRIORITY_SMP_ENTRY_POINTS \
  { \
    _Scheduler_priority_SMP_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_priority_SMP_Yield, \
    _Scheduler_priority_SMP_Block, \
    _Scheduler_priority_SMP_Unblock, \
    _Scheduler_priority_SMP_Change_priority, \
    _Scheduler_priority_SMP_Ask_for_help, \
    _Scheduler_priority_SMP_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_priority_SMP_Update_priority, \
    _Scheduler_priority_Priority_compare, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Tick, \
    _Scheduler_SMP_Start_idle \
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
  }

void _Scheduler_priority_SMP_Initialize( const Scheduler_Control *scheduler );

void _Scheduler_priority_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

Thread_Control *_Scheduler_priority_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

Thread_Control *_Scheduler_priority_SMP_Change_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority,
  bool                     prepend_it
);

Thread_Control *_Scheduler_priority_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *needs_help,
  Thread_Control          *offers_help
);

void _Scheduler_priority_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control *thread,
  Priority_Control new_priority
);

Thread_Control *_Scheduler_priority_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYSMP_H */
