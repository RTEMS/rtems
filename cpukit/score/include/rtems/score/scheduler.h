/**
 *  @file  rtems/score/scheduler.h
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
#include <rtems/score/prioritybitmap.h>

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
  void ( *schedule )(void);

  /** Voluntarily yields the processor per the scheduling policy. */
  void ( *yield )(void);

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
 * Macro testing whether @a p1 has lower priority than @a p2
 * in the intuitive sense of priority.
 */
#define _Scheduler_Is_priority_lower_than( _p1, _p2 ) \
  (_Scheduler_Priority_compare(_p1,_p2) < 0)

/**
 * Macro testing whether @a p1 has higher priority than @a p2
 * in the intuitive sense of priority.
 */
#define _Scheduler_Is_priority_higher_than( _p1, _p2 ) \
  (_Scheduler_Priority_compare(_p1,_p2) > 0)

/**
 *  This routine initializes the scheduler to the policy chosen by the user
 *  through confdefs, or to the priority scheduler with ready chains by
 *  default.
 */
void _Scheduler_Handler_initialization( void );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/scheduler.inl>
#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
