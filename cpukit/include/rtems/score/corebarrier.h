/**
 * @file
 *
 * @ingroup RTEMSScoreBarrier
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreBarrier which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
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
 * @defgroup RTEMSScoreBarrier Barrier Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Barrier Handler implementation.
 *
 * This handler encapsulates functionality which provides the foundation
 * Barrier services used in all of the APIs supported by RTEMS.
 *
 * @{
 */

/**
 * @brief This control block is used to manage a barrier.
 */
typedef struct {
  /**
   * @brief This member is used to manage the set of tasks which are
   *   blocked waiting for the barrier to be released.
   */
  Thread_queue_Control Wait_queue;

  /**
   * @brief This member contains the current number of thread waiting at the
   *   barrier to be released.
   */
  uint32_t number_of_waiting_threads;

  /**
   * @brief This member indicates the number of threads which must arrive at
   *   the barrier to trip the automatic release.
   *
   * Use ::CORE_BARRIER_MANUAL_RELEASE_MAXIMUM_COUNT to indicate a manual
   * release barrier.
   */
  uint32_t maximum_count;
} CORE_barrier_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
