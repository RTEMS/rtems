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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_THREADSUP_H
#define _RTEMS_POSIX_THREADSUP_H

#include <rtems/score/coresem.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
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

/**
 * This defines the POSIX API support structure associated with
 * each thread in a system with POSIX configured.
 */
typedef struct {
  /** Back pointer to thread of this POSIX API control. */
  Thread_Control         *thread;
  /** This is the POSIX threads attribute set. */
  pthread_attr_t          Attributes;
  /** This is the thread's current scheduling policy. */
  int                     schedpolicy;
  /** This is the thread's current set of scheduling parameters. */
  struct sched_param      schedparam;
  /**
   * This is the timer which controls when the thread executes at
   * high and low priority when using the sporadic scheduler.
   */
  Watchdog_Control        Sporadic_timer;

  /** This is the set of signals which are currently unblocked. */
  sigset_t                signals_unblocked;
  /** This is the set of signals which are currently pending. */
  sigset_t                signals_pending;

  /**
   * @brief Signal post-switch action in case signals are pending.
   */
  Thread_Action           Signal_action;
} POSIX_API_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
