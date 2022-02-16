/* SPDX-License-Identifier: BSD-2-Clause */

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
