/**
 *  @file  rtems/score/corespinlock.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORESPINLOCK_H
#define _RTEMS_SCORE_CORESPINLOCK_H

/**
 *  @defgroup ScoreSpinlock Spinlock Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Spinlock services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/thread.h>
#include <rtems/score/priority.h>

/**
 *  Core Spinlock handler return statuses.
 */
typedef enum {
  /** This status indicates that the operation completed successfully. */
  CORE_SPINLOCK_SUCCESSFUL,
  /** This status indicates that the current thread already holds the spinlock.
   *  An attempt to relock it will result in deadlock.
   */
  CORE_SPINLOCK_HOLDER_RELOCKING,
  /** This status indicates that the current thread is attempting to unlock a
   *  spinlock that is held by another thread.
   */
  CORE_SPINLOCK_NOT_HOLDER,
  /** This status indicates that a thread reached the limit of time it
   *  was willing to wait on the spin lock.
   */
  CORE_SPINLOCK_TIMEOUT,
  /** This status indicates that a thread is currently waiting for this
   *  spin lock.
   */
  CORE_SPINLOCK_IS_BUSY,
  /** This status indicates that the spinlock is currently locked and thus
   *  unavailable.
   */
  CORE_SPINLOCK_UNAVAILABLE,
  /** This status indicates that the spinlock is not currently locked and thus
   *  should not be released.
   */
  CORE_SPINLOCK_NOT_LOCKED
}   CORE_spinlock_Status;

/** This is a shorthand for the last status code. */
#define CORE_SPINLOCK_STATUS_LAST CORE_SPINLOCK_NOT_LOCKED

/** This indicates the lock is available. */
#define CORE_SPINLOCK_UNLOCKED 0

/** This indicates the lock is unavailable. */
#define CORE_SPINLOCK_LOCKED   1

/**
 *  The following defines the control block used to manage the
 *  attributes of each spinlock.
 */
typedef struct {
  /** This element indicates XXX
   */
  uint32_t                  XXX;
}   CORE_spinlock_Attributes;

/**
 *  The following defines the control block used to manage each
 *  spinlock.
 */
typedef struct {
  /** XXX may not be needed */
  CORE_spinlock_Attributes  Attributes;

  /** This field is the lock.
   */
  volatile uint32_t     lock;

  /** This field is a count of the current number of threads using
   *  this spinlock.  It includes the thread holding the lock as well
   *  as those waiting.
   */
  volatile uint32_t     users;

  /** This field is the Id of the thread holding the lock.  It may or may
   *  not be the thread which acquired it.
   */
  volatile Objects_Id   holder;
}   CORE_spinlock_Control;

/**
 *  This routine initializes the spinlock based on the parameters passed.
 *
 *  @param[in] the_spinlock is the spinlock to initialize
 *  @param[in] the_spinlock_attributes define the behavior of this instance
 */
void _CORE_spinlock_Initialize(
  CORE_spinlock_Control       *the_spinlock,
  CORE_spinlock_Attributes    *the_spinlock_attributes
);

/**
 *  This routine wait for the spinlock to be released.  If the spinlock
 *  is set to automatic and this is the appropriate thread, then it returns
 *  immediately.  Otherwise, the calling thread is blocked until the spinlock
 *  is released.
 *
 *  @param[in] the_spinlock is the spinlock to wait for
 *  @param[in] wait is true if willing to wait
 *  @param[in] timeout is the maximum number of ticks to spin (0 is forever)
 *
 * @return A status is returned which indicates the success or failure of
 *         this operation.
 */
CORE_spinlock_Status _CORE_spinlock_Wait(
  CORE_spinlock_Control  *the_spinlock,
  bool                    wait,
  Watchdog_Interval       timeout
);

/**
 *  This routine manually releases the spinlock.  All of the threads waiting
 *  for the spinlock will be readied.
 *
 *  @param[in] the_spinlock is the spinlock to surrender
 */
CORE_spinlock_Status _CORE_spinlock_Release(
  CORE_spinlock_Control *the_spinlock
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/corespinlock.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
