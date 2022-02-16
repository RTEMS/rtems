/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief POSIX Thread API Support
 *
 * This defines the POSIX thread API extension.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#ifndef _RTEMS_POSIX_THREADSUP_H
#define _RTEMS_POSIX_THREADSUP_H

#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

#include <pthread.h>
#include <signal.h>

/**
 *  @defgroup POSIX_THREAD POSIX Thread API Extension
 *
 *  @ingroup POSIXAPI
 * 
 */
/**@{**/
#ifdef __cplusplus
extern "C" {
#endif

#if defined(RTEMS_POSIX_API)
/**
 * This defines the POSIX API support structure associated with
 * each thread in a system with POSIX configured.
 */
typedef struct {
  /**
   * @brief Control block for the sporadic server scheduling policy.
   */
  struct {
    /** The thread of this sporadic control block */
    Thread_Control *thread;

    /**
     * @brief This is the timer which controls when the thread executes at high
     * and low priority when using the sporadic server scheduling policy.
     */
    Watchdog_Control Timer;

    /**
     * @brief The low priority when using the sporadic server scheduling
     * policy.
     */
    Priority_Node Low_priority;

    /**
     * @brief Replenishment period for sporadic server.
     */
    struct timespec sched_ss_repl_period;

    /**
     * @brief Initial budget for sporadic server.
     */
    struct timespec sched_ss_init_budget;

    /**
     * @brief Maximum pending replenishments.
     *
     * Only used by pthread_getschedparam() and pthread_getattr_np().
    */
    int sched_ss_max_repl;
  } Sporadic;

  /** This is the set of signals which are currently unblocked. */
  sigset_t                signals_unblocked;
  /** This is the set of signals which are currently pending. */
  sigset_t                signals_pending;

  /**
   * @brief Signal post-switch action in case signals are pending.
   */
  Thread_Action           Signal_action;
} POSIX_API_Control;
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
