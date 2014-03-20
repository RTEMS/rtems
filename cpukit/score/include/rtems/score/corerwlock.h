/**
 *  @file  rtems/score/corerwlock.h
 *
 *  @brief Constants and Structures Associated with the RWLock Handler
 *
 *  This include file contains all the constants and structures associated
 *  with the RWLock Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORERWLOCK_H
#define _RTEMS_SCORE_CORERWLOCK_H

#include <rtems/score/threadq.h>

/**
 *  @defgroup ScoreRWLock RWLock Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  RWLock services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  RWLock State.
 */
typedef enum {
  /** This indicates the the RWLock is not currently locked.
   */
  CORE_RWLOCK_UNLOCKED,
  /** This indicates the the RWLock is currently locked for reading.
   */
  CORE_RWLOCK_LOCKED_FOR_READING,
  /** This indicates the the RWLock is currently locked for reading.
   */
  CORE_RWLOCK_LOCKED_FOR_WRITING
}   CORE_RWLock_States;

/**
 *  The following defines the control block used to manage the
 *  attributes of each RWLock.
 */
typedef struct {
  /** This field indicates XXX.
   */
  int XXX;
}   CORE_RWLock_Attributes;

/**
 *  The following defines the control block used to manage each
 *  RWLock.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting for the RWLock to be released.
   */
  Thread_queue_Control     Wait_queue;
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_RWLock_Attributes  Attributes;
  /** This element is the current state of the RWLock.
   */
  CORE_RWLock_States       current_state;
  /** This element contains the current number of thread waiting for this
   *  RWLock to be released. */
  uint32_t                 number_of_readers;
}   CORE_RWLock_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
