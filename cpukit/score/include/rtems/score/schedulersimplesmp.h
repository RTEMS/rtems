/**
 *  @file  rtems/score/schedulersimplesmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of threads on a simple-priority-based ready queue.
 *  This implementation is SMP-aware and schedules across multiple cores.
 *
 *  The implementation relies heavily on the Simple Scheduler and
 *  only replaces a few routines from that scheduler.
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_SMP_H
#define _RTEMS_SCORE_SCHEDULERSIMPLE_SMP_H

/**
 *  @addtogroup ScoreScheduler
 *
 *  The Simple SMP Scheduler attempts to faithfully implement the
 *  behaviour of the Deterministic Priority Scheduler while spreading
 *  the threads across multiple cores.  It takes into account thread
 *  priority, preemption, and how long a thread has been executing
 *  on a core as factors.  From an implementation perspective, it
 *  relies heavily on the Simple Priority Scheduler.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulersimple.h>
#include <rtems/score/schedulerpriority.h>

/**
 *  Entry points for Scheduler Simple SMP
 */
#define SCHEDULER_SIMPLE_SMP_ENTRY_POINTS \
  { \
    _Scheduler_simple_Initialize,         /* initialize entry point */ \
    _Scheduler_simple_smp_Schedule,       /* schedule entry point */ \
    _Scheduler_simple_Yield,              /* yield entry point */ \
    _Scheduler_simple_smp_Block,          /* block entry point */ \
    _Scheduler_simple_smp_Unblock,        /* unblock entry point */ \
    _Scheduler_simple_Allocate,           /* allocate entry point */ \
    _Scheduler_simple_Free,               /* free entry point */ \
    _Scheduler_simple_Update,             /* update entry point */ \
    _Scheduler_simple_Enqueue,            /* enqueue entry point */ \
    _Scheduler_simple_Enqueue_first,      /* enqueue_first entry point */ \
    _Scheduler_simple_Extract,            /* extract entry point */ \
    _Scheduler_priority_Priority_compare, /* compares two priorities */ \
    _Scheduler_priority_Release_job,      /* new period of task */ \
    _Scheduler_simple_smp_Tick            /* tick entry point */ \
  }

/**
 *  @brief Scheduler Simple SMP Schedule Method
 *
 *  This routine allocates ready threads to individual cores in an SMP
 *  system.  If the allocation results in a new heir which requires
 *  a dispatch, then the dispatch needed flag for that core is set.
 */
void _Scheduler_simple_smp_Schedule( void );

/**
 *  @brief Scheduler Simple SMP Block Method
 *
 *  This routine removes @a the_thread from the scheduling decision,
 *  that is, removes it from the ready queue.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  @param[in] the_thread is the thread that is to be blocked
 */
void _Scheduler_simple_smp_Block(
  Thread_Control *the_thread
);

/**
 *  @brief Scheduler Simple SMP Unblock Method
 *
 *  This routine adds @a the_thread to the scheduling decision,
 *  that is, adds it to the ready queue and updates any appropriate
 *  scheduling variables, for example the heir thread.
 *
 *  @param[in] the_thread is the thread that is to be unblocked
 */
void _Scheduler_simple_smp_Unblock(
  Thread_Control *the_thread
);

/**
 *  @brief Scheduler Simple SMP Tick Method
 *
 *  This routine is invoked as part of processing each clock tick.
 *  It is responsible for determining if the current thread allows
 *  timeslicing and, if so, when its timeslice expires.
 */
void _Scheduler_simple_smp_Tick( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
