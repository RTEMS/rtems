/**
 * @file
 *
 * @ingroup RTEMSScoreSemaphore
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSemaphore which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
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
 * @defgroup RTEMSScoreSemaphore Semaphore Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Semaphore Handler implementation.
 *
 * This handler encapsulates functionality which provides the foundation
 * Semaphore services used in all of the APIs supported by RTEMS.
 */
/** @{ */

/**
 *  The following defines the control block used to manage each
 *  counting semaphore.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting to obtain the semaphore.
   */
  Thread_queue_Control        Wait_queue;

  /** This element contains the current count of this semaphore. */
  uint32_t                    count;
}   CORE_semaphore_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
