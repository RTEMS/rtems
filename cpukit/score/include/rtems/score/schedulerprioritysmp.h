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
 * http://www.rtems.com/license/LICENSE.
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
 * @ingroup ScoreScheduler
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
 * @brief Entry points for the Simple SMP Scheduler.
 */
#define SCHEDULER_PRIORITY_SMP_ENTRY_POINTS \
  { \
    _Scheduler_priority_SMP_Initialize, \
    _Scheduler_priority_SMP_Schedule, \
    _Scheduler_priority_SMP_Yield, \
    _Scheduler_priority_SMP_Block, \
    _Scheduler_priority_SMP_Enqueue_fifo, \
    _Scheduler_priority_Allocate, \
    _Scheduler_priority_Free, \
    _Scheduler_priority_SMP_Update, \
    _Scheduler_priority_SMP_Enqueue_fifo, \
    _Scheduler_priority_SMP_Enqueue_lifo, \
    _Scheduler_priority_SMP_Extract, \
    _Scheduler_priority_Priority_compare, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Tick, \
    _Scheduler_SMP_Start_idle \
  }

void _Scheduler_priority_SMP_Initialize( void );

void _Scheduler_priority_SMP_Schedule( Thread_Control *thread );

void _Scheduler_priority_SMP_Block( Thread_Control *thread );

void _Scheduler_priority_SMP_Update( Thread_Control *thread );

void _Scheduler_priority_SMP_Enqueue_fifo( Thread_Control *thread );

void _Scheduler_priority_SMP_Enqueue_lifo( Thread_Control *thread );

void _Scheduler_priority_SMP_Extract( Thread_Control *thread );

void _Scheduler_priority_SMP_Yield( Thread_Control *thread );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERPRIORITYSMP_H */
