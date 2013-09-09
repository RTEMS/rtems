/**
 *  @file  rtems/score/scheduler.h
 *
 *  @brief Constants and Structures Associated with the Scheduler
 *
 *  This include file contains all the constants and structures associated
 *  with the scheduler.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULER_H
#define _RTEMS_SCORE_SCHEDULER_H

#include <rtems/score/percpu.h>
#include <rtems/score/chain.h>
#include <rtems/score/priority.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreScheduler Scheduler Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to managing sets of threads
 *  that are ready for execution.
 */
/**@{*/

/**
 * function jump table that holds pointers to the functions that
 * implement specific schedulers.
 */
typedef struct {
  /** Implements the scheduling decision logic (policy). */
  void ( *initialize )(void);

  /** Implements the scheduling decision logic (policy). */
  void ( *schedule )( Thread_Control *thread );

  /**
   * @brief Voluntarily yields the processor per the scheduling policy.
   *
   * @see _Scheduler_Yield().
   */
  void ( *yield )( Thread_Control *thread );

  /** Removes the given thread from scheduling decisions. */
  void ( *block )(Thread_Control *);

  /** Adds the given thread to scheduling decisions. */
  void ( *unblock )(Thread_Control *);

  /** allocates the scheduler field of the given thread */
  void * ( *allocate )(Thread_Control *);

  /** frees the scheduler field of the given thread */
  void ( *free )(Thread_Control *);

  /** updates the scheduler field of the given thread -- primarily used
   * when changing the thread's priority. */
  void ( *update )(Thread_Control *);

  /** enqueue a thread as the last of its priority group */
  void ( *enqueue )(Thread_Control *);

  /** enqueue a thread as the first of its priority group */
  void ( *enqueue_first )(Thread_Control *);

  /** extract a thread from the ready set */
  void ( *extract )(Thread_Control *);

  /**
   * Compares two priorities (returns >0 for higher priority, 0 for equal
   * and <0 for lower priority).
   */
  int ( *priority_compare )(Priority_Control, Priority_Control);

  /** This routine is called upon release of a new job. */
  void ( *release_job ) (Thread_Control *, uint32_t);

  /** perform scheduler update actions required at each clock tick */
  void ( *tick )(void);

  /**
   * @brief Starts the idle thread for a particular processor.
   *
   * @see _Scheduler_Start_idle().
   */
  void ( *start_idle )( Thread_Control *thread, Per_CPU_Control *processor );
} Scheduler_Operations;

/**
 * This is the structure used to manage the scheduler.
 */
typedef struct {
  /**
   *  This points to the data structure used to manage the ready set of
   *  tasks. The pointer varies based upon the type of
   *  ready queue required by the scheduler.
   */
  void                   *information;

  /** The jump table for scheduler-specific functions */
  Scheduler_Operations    Operations;
} Scheduler_Control;

/**
 *  The _Scheduler holds the structures used to manage the
 *  scheduler.
 *
 * @note Can we make this per-cpu? then _Scheduler will be a macro.
 *
 * @note This is instantiated and initialized in confdefs.h.
 */
extern Scheduler_Control  _Scheduler;

/**
 * @brief Returns an arbitrary non-NULL value.
 *
 * @param[in] thread Unused.
 *
 * @return An arbitrary non-NULL value.
 */
void *_Scheduler_default_Allocate(
  Thread_Control *thread
);

/**
 * @brief Does nothing.
 *
 * @param[in] thread Unused.
 */
void _Scheduler_default_Free(
  Thread_Control *thread
);

/**
 * @brief Does nothing.
 *
 * @param[in] thread Unused.
 */
void _Scheduler_default_Update(
  Thread_Control *the_thread
);

/**
 * @brief Does nothing.
 *
 * @param[in] thread Unused.
 * @param[in] deadline Unused.
 */
void _Scheduler_default_Release_job(
  Thread_Control *thread,
  uint32_t        deadline
);

/**
 * @brief Performs tick operations depending on the CPU budget algorithm for
 * each executing thread.
 *
 * This routine is invoked as part of processing each clock tick.
 */
void _Scheduler_default_Tick( void );

/**
 * @brief Unblocks the thread.
 *
 * @param[in,out] thread An idle thread.
 * @param[in] processor This parameter is unused.
 */
void _Scheduler_default_Start_idle(
  Thread_Control  *thread,
  Per_CPU_Control *processor
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
