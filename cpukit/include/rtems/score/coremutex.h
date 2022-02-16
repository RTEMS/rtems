/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMutex
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreMutex which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_SCORE_COREMUTEX_H
#define _RTEMS_SCORE_COREMUTEX_H

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/interr.h>

struct _Scheduler_Control;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreMutex Mutex Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Mutex Handler implementation.
 *
 * This handler encapsulates functionality which provides the foundation
 * Mutex services used in all of the APIs supported by RTEMS.  A mutex is an
 * enhanced version of the standard Dijkstra binary semaphore used to provide
 * synchronization and mutual exclusion capabilities.
 *
 * @{
 */

/**
 *  @brief Control block used to manage each mutex.
 *
 *  The following defines the control block used to manage each mutex.
 */
typedef struct {
  /**
   * @brief The thread queue of this mutex.
   *
   * The owner of the thread queue indicates the mutex owner.
   */
  Thread_queue_Control Wait_queue;
} CORE_mutex_Control;

/**
 * @brief The recursive mutex control.
 */
typedef struct {
  /**
   * @brief The plain non-recursive mutex.
   */
  CORE_mutex_Control Mutex;

  /**
   * @brief The nest level in case of a recursive seize.
   */
  unsigned int nest_level;
} CORE_recursive_mutex_Control;

/**
 * @brief The recursive mutex control with priority ceiling protocol support.
 */
typedef struct {
  /**
   * @brief The plain recursive mutex.
   */
  CORE_recursive_mutex_Control Recursive;

  /**
   * @brief The priority ceiling node for the mutex owner.
   */
  Priority_Node Priority_ceiling;

#if defined(RTEMS_SMP)
  /**
   * @brief The scheduler instance for this priority ceiling mutex.
   */
  const struct _Scheduler_Control *scheduler;
#endif
} CORE_ceiling_mutex_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
