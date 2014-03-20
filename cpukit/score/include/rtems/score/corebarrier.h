/**
 *  @file rtems/score/corebarrier.h
 *
 *  @brief Constants and Structures Associated with the Barrier Handler
 *
 *  This include file contains all the constants and structures associated
 *  with the Barrier Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREBARRIER_H
#define _RTEMS_SCORE_COREBARRIER_H

#include <rtems/score/threadq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreBarrier Barrier Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Barrier services used in all of the APIs supported by RTEMS.
 */
/**@{*/

/**
 *  Flavors of barriers.
 */
typedef enum {
  /** This specifies that the barrier will automatically release when
   *  the user specified number of threads have arrived at the barrier.
   */
  CORE_BARRIER_AUTOMATIC_RELEASE,
  /** This specifies that the user will have to manually release the barrier
   *  in order to release the waiting threads.
   */
  CORE_BARRIER_MANUAL_RELEASE
}   CORE_barrier_Disciplines;

/**
 *  The following defines the control block used to manage the
 *  attributes of each barrier.
 */
typedef struct {
  /** This field indicates whether the barrier is automatic or manual.
   */
  CORE_barrier_Disciplines  discipline;
  /** This element indicates the number of threads which must arrive at the
   *  barrier to trip the automatic release.
   */
  uint32_t                  maximum_count;
}   CORE_barrier_Attributes;

/**
 *  The following defines the control block used to manage each
 *  barrier.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting for the barrier to be released.
   */
  Thread_queue_Control     Wait_queue;
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_barrier_Attributes  Attributes;
  /** This element contains the current number of thread waiting for this
   *  barrier to be released. */
  uint32_t                 number_of_waiting_threads;
}   CORE_barrier_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
