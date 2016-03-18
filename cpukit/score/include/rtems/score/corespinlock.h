/**
 *  @file  rtems/score/corespinlock.h
 *
 *  @brief Constants and Structures Associated with the Spinlock Handler
 *
 *  This include file contains all the constants and structures associated
 *  with the Spinlock Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORESPINLOCK_H
#define _RTEMS_SCORE_CORESPINLOCK_H

#include <rtems/score/isrlock.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreSpinlock Spinlock Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Spinlock services used in all of the APIs supported by RTEMS.
 */
/**@{*/

/**
 *  The following defines the control block used to manage each
 *  spinlock.
 */
typedef struct {
  /**
   * @brief Lock to protect the other fields.
   *
   * This implementation is a bit stupid.  However, test cases in the Linux
   * Test Project do things like sleep() and printf() while owning a
   * pthread_spinlock_t, e.g.
   * testcases/open_posix_testsuite/conformance/interfaces/pthread_spin_lock/1-2.c
   */
  ISR_LOCK_MEMBER( Lock )

  /** This field is the lock.
   */
  uint32_t lock;

  /** This field is a count of the current number of threads using
   *  this spinlock.  It includes the thread holding the lock as well
   *  as those waiting.
   */
  uint32_t users;

  /** This field is the Id of the thread holding the lock.  It may or may
   *  not be the thread which acquired it.
   */
  Thread_Control *holder;
}   CORE_spinlock_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
