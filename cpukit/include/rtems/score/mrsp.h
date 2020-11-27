/**
 * @file
 *
 * @ingroup RTEMSScoreMRSP
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreMRSP which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MRSP_H
#define _RTEMS_SCORE_MRSP_H

#include <rtems/score/cpuopts.h>
#include <rtems/score/threadq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreMRSP Multiprocessor Resource Sharing Protocol (MrsP)
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to support the Multiprocessor
 *   Resource Sharing Protocol (MrsP) .
 *
 * The Multiprocessor Resource Sharing Protocol (MrsP) is defined in A.  Burns
 * and A.J.  Wellings, A Schedulability Compatible Multiprocessor Resource
 * Sharing Protocol - MrsP, Proceedings of the 25th Euromicro Conference on
 * Real-Time Systems (ECRTS 2013), July 2013.  It is a generalization of the
 * Priority Ceiling Protocol to SMP systems.  Each MrsP semaphore uses a
 * ceiling priority per scheduler instance.  A task obtaining or owning a MrsP
 * semaphore will execute with the ceiling priority for its scheduler instance
 * as specified by the MrsP semaphore object.  Tasks waiting to get ownership
 * of a MrsP semaphore will not relinquish the processor voluntarily.  In case
 * the owner of a MrsP semaphore gets preempted it can ask all tasks waiting
 * for this semaphore to help out and temporarily borrow the right to execute
 * on one of their assigned processors.
 *
 * @{
 */

#if defined(RTEMS_SMP)

/**
 * @brief MrsP control block.
 */
typedef struct {
  /**
   * @brief The thread queue to manage ownership and waiting threads.
   */
  Thread_queue_Control Wait_queue;

  /**
   * @brief The ceiling priority used by the owner thread.
   */
  Priority_Node Ceiling_priority;

  /**
   * @brief One ceiling priority per scheduler instance.
   */
  Priority_Control ceiling_priorities[ RTEMS_ZERO_LENGTH_ARRAY ];
} MRSP_Control;

#endif /* RTEMS_SMP */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_MRSP_H */
