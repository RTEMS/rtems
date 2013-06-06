/**
 * @file
 *
 * @brief Simple SMP Scheduler API
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2013 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_SMP_H
#define _RTEMS_SCORE_SCHEDULERSIMPLE_SMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulersimple.h>
#include <rtems/score/schedulerpriority.h>

/**
 * @defgroup ScoreSchedulerSMP Simple SMP Scheduler
 *
 * @ingroup ScoreScheduler
 *
 * The Simple SMP Scheduler allocates a processor for the processor count
 * highest priority ready threads.  The thread priority and position in the
 * ready chain are the only information to determine the scheduling decision.
 * Threads with an allocated processor are in the scheduled chain.  After
 * initialization the scheduled chain has exactly processor count nodes.  Each
 * processor has exactly one allocated thread after initialization.  All
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
  Chain_Control ready;
  Chain_Control scheduled;
} Scheduler_simple_smp_Control;

/**
 * @brief Entry points for the Simple SMP Scheduler.
 */
#define SCHEDULER_SIMPLE_SMP_ENTRY_POINTS \
  { \
    _Scheduler_simple_smp_Initialize, \
    _Scheduler_simple_smp_Schedule, \
    _Scheduler_simple_smp_Yield, \
    _Scheduler_simple_smp_Extract, \
    _Scheduler_simple_smp_Enqueue_priority_fifo, \
    _Scheduler_simple_Allocate, \
    _Scheduler_simple_Free, \
    _Scheduler_simple_Update, \
    _Scheduler_simple_smp_Enqueue_priority_fifo, \
    _Scheduler_simple_smp_Enqueue_priority_lifo, \
    _Scheduler_simple_smp_Extract, \
    _Scheduler_priority_Priority_compare, \
    _Scheduler_priority_Release_job, \
    _Scheduler_default_Tick, \
    _Scheduler_simple_smp_Start_idle \
  }

void _Scheduler_simple_smp_Initialize( void );

void _Scheduler_simple_smp_Enqueue_priority_fifo( Thread_Control *thread );

void _Scheduler_simple_smp_Enqueue_priority_lifo( Thread_Control *thread );

void _Scheduler_simple_smp_Extract( Thread_Control *thread );

void _Scheduler_simple_smp_Yield( Thread_Control *thread );

void _Scheduler_simple_smp_Schedule( void );

void _Scheduler_simple_smp_Start_idle(
  Thread_Control *thread,
  Per_CPU_Control *cpu
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
