/**
 *  @file  rtems/score/threadsync.h
 *
 *  This include file contains all constants and structures associated
 *  with synchronizing a thread blocking operation with potential
 *  actions in an ISR.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREAD_SYNC_H
#define _RTEMS_SCORE_THREAD_SYNC_H

/**
 *  @defgroup ScoreThreadSync Thread Blocking Operation Synchronization Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to the management of
 *  synchronization critical sections during blocking operations.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following enumerated types indicate what happened while the thread
 *  blocking was in the synchronization window.
 */
typedef enum {
  THREAD_BLOCKING_OPERATION_SYNCHRONIZED,
  THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED,
  THREAD_BLOCKING_OPERATION_TIMEOUT,
  THREAD_BLOCKING_OPERATION_SATISFIED
}  Thread_blocking_operation_States;

/*
 *  Operations require a thread pointer so they are prototyped
 *  in thread.h
 */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
