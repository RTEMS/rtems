/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Copyright (C) 2014, 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
