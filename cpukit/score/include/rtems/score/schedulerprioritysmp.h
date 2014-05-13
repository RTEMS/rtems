/**
 * @file
 *
 * @ingroup ScoreSchedulerPrioritySMP
 *
 * @brief Deterministic Priority SMP Scheduler API
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

typedef struct {
  Scheduler_SMP_Context    Base;
  Priority_bit_map_Control Bit_map;
  Chain_Control            Ready[ 0 ];
} Scheduler_priority_SMP_Context;

/**
 * @brief Entry points for the Priority SMP Scheduler.
 */
#define SCHEDULER_PRIORITY_SMP_ENTRY_POINTS \
  { \
    _Scheduler_priority_SMP_Initialize, \
    _Scheduler_priority_SMP_Schedule, \
    _Scheduler_priority_SMP_Yield, \
    _Scheduler_priority_SMP_Block, \
    _Scheduler_priority_SMP_Enqueue_fifo, \
    _Scheduler_default_Allocate, \
    _Scheduler_default_Free, \
    _Scheduler_priority_SMP_Update, \
    _Scheduler_priority_SMP_Enqueue_fifo, \
    _Scheduler_priority_SMP_Enqueue_lifo, \
    _Scheduler_priority_SMP_Extract, \
    _Scheduler_priority_Priority_compare, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Tick, \
    _Scheduler_priority_SMP_Start_idle, \
    _Scheduler_default_Get_affinity, \
    _Scheduler_default_Set_affinity \
  }

void _Scheduler_priority_SMP_Initialize( const Scheduler_Control *scheduler );

void _Scheduler_priority_SMP_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Update(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Enqueue_fifo(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Enqueue_lifo(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Extract(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
);

void _Scheduler_priority_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control *thread,
  Per_CPU_Control *cpu
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYSMP_H */
