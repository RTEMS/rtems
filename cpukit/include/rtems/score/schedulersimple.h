/**
 *  @file  rtems/score/schedulersimple.h
 *
 *  @brief Manipulation of Threads Simple-Priority-Based Ready Queue
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of threads on a simple-priority-based ready queue.
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_H
#define _RTEMS_SCORE_SCHEDULERSIMPLE_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreSchedulerSimple Simple Priority Scheduler
 *
 *  @ingroup ScoreScheduler
 */
/**@{*/

#define SCHEDULER_SIMPLE_MAXIMUM_PRIORITY 255

/**
 *  Entry points for Scheduler Simple
 */
#define SCHEDULER_SIMPLE_ENTRY_POINTS \
  { \
    _Scheduler_simple_Initialize,         /* initialize entry point */ \
    _Scheduler_simple_Schedule,           /* schedule entry point */ \
    _Scheduler_simple_Yield,              /* yield entry point */ \
    _Scheduler_simple_Block,              /* block entry point */ \
    _Scheduler_simple_Unblock,            /* unblock entry point */ \
    _Scheduler_simple_Update_priority,    /* update priority entry point */ \
    _Scheduler_default_Map_priority,      /* map priority entry point */ \
    _Scheduler_default_Unmap_priority,    /* unmap priority entry point */ \
    SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_default_Node_initialize,   /* node initialize entry point */ \
    _Scheduler_default_Node_destroy,      /* node destroy entry point */ \
    _Scheduler_default_Release_job,       /* new period of task */ \
    _Scheduler_default_Cancel_job,        /* cancel period of task */ \
    _Scheduler_default_Tick,              /* tick entry point */ \
    _Scheduler_default_Start_idle         /* start idle entry point */ \
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
  }

/**
 * @brief Simple scheduler context.
 */
typedef struct {
  /**
   * @brief Basic scheduler context.
   */
  Scheduler_Context Base;

  /**
   * @brief One ready queue for all ready threads.
   */
  Chain_Control Ready;
} Scheduler_simple_Context;

/**
 *  @brief Initialize simple scheduler.
 *
 *  This routine initializes the simple scheduler.
 */
void _Scheduler_simple_Initialize( const Scheduler_Control *scheduler );

/**
 *  This routine sets the heir thread to be the next ready thread
 *  on the ready queue by getting the first node in the scheduler
 *  information.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread causing the scheduling operation.
 */
void _Scheduler_simple_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

void _Scheduler_simple_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_simple_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_simple_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

void _Scheduler_simple_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
