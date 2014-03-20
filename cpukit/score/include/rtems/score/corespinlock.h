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

#include <rtems/score/object.h>

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

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
