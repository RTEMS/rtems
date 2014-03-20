/**
 *  @file  rtems/score/coresem.h
 *
 *  @brief Data Associated with the Counting Semaphore Handler
 *
 *  This include file contains all the constants and structures associated
 *  with the Counting Semaphore Handler.  A counting semaphore is the
 *  standard Dijkstra binary semaphore used to provide synchronization
 *  and mutual exclusion capabilities.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORESEM_H
#define _RTEMS_SCORE_CORESEM_H

#include <rtems/score/threadq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreSemaphore Semaphore Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Semaphore services used in all of the APIs supported by RTEMS.
 */
/**@{*/

/**
 *  Blocking disciplines for a semaphore.
 */
typedef enum {
  /** This specifies that threads will wait for the semaphore in FIFO order. */
  CORE_SEMAPHORE_DISCIPLINES_FIFO,
  /** This specifies that threads will wait for the semaphore in
   *  priority order.
   */
  CORE_SEMAPHORE_DISCIPLINES_PRIORITY
}   CORE_semaphore_Disciplines;

/**
 *  The following defines the control block used to manage the
 *  attributes of each semaphore.
 */
typedef struct {
  /** This element indicates the maximum count this semaphore may have. */
  uint32_t                    maximum_count;
  /** This field indicates whether threads waiting on the semaphore block in
   *  FIFO or priority order.
   */
  CORE_semaphore_Disciplines  discipline;
}   CORE_semaphore_Attributes;

/**
 *  The following defines the control block used to manage each
 *  counting semaphore.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting to obtain the semaphore.
   */
  Thread_queue_Control        Wait_queue;
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_semaphore_Attributes   Attributes;
  /** This element contains the current count of this semaphore. */
  uint32_t                    count;
}   CORE_semaphore_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
