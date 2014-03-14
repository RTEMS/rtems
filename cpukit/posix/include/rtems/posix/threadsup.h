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
  /** This is the POSIX threads attribute set. */
  pthread_attr_t          Attributes;
  /** This indicates whether the thread is attached or detached. */
  int                     detachstate;
  /** This is the set of threads waiting for the thread to exit. */
  Thread_queue_Control    Join_List;
  /** This is the thread's current scheduling policy. */
  int                     schedpolicy;
  /** This is the thread's current set of scheduling parameters. */
  struct sched_param      schedparam;
  /**
   * This is the high priority to execute at when using the sporadic
   * scheduler.
   */
  int                     ss_high_priority;
  /**
   * This is the timer which controls when the thread executes at
   * high and low priority when using the sporadic scheduler.
   */
  Watchdog_Control        Sporadic_timer;

  /** This is the set of signals which are currently blocked. */
  sigset_t                signals_blocked;
  /** This is the set of signals which are currently pending. */
  sigset_t                signals_pending;

  /**
   * @brief Signal post-switch action in case signals are pending.
   */
  Thread_Action           Signal_action;

  /*******************************************************************/
  /*******************************************************************/
  /***************         POSIX Cancelability         ***************/
  /*******************************************************************/
  /*******************************************************************/

  /** This is the cancelability state. */
  int                     cancelability_state;
  /** This is the cancelability type. */
  int                     cancelability_type;
  /** This indicates if a cancelation has been requested. */
  int                     cancelation_requested;
#ifndef HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT
  /** This is the set of cancelation handlers. */
  Chain_Control           Cancellation_Handlers;
#else /* HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT */
  /**
   * @brief LIFO list of cleanup contexts.
   */
  struct _pthread_cleanup_context *last_cleanup_context;
#endif /* HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT */

} POSIX_API_Control;

/**
 * @brief POSIX thread exit shared helper.
 *
 * 16.1.5.1 Thread Termination, p1003.1c/Draft 10, p. 150
 *
 * This method is a helper routine which ensures that all
 * POSIX thread calls which result in a thread exiting will
 * do so in the same manner.
 *
 * @param[in] the_thread is a pointer to the thread exiting or being canceled
 * @param[in] value_ptr is a pointer the value to be returned by the thread
 *
 * NOTE: Key destructors are executed in the POSIX api delete extension.
 *
 */
void _POSIX_Thread_Exit(
  Thread_Control *the_thread,
  void           *value_ptr
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
