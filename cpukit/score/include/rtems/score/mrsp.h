/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MRSP_H
#define _RTEMS_SCORE_MRSP_H

#include <rtems/score/cpuopts.h>

#if defined(RTEMS_SMP)

#include <rtems/score/atomic.h>
#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreMRSP Multiprocessor Resource Sharing Protocol Handler
 *
 * @ingroup Score
 *
 * @brief Multiprocessor Resource Sharing Protocol (MrsP).
 *
 * The Multiprocessor Resource Sharing Protocol (MrsP) is defined in A.  Burns
 * and A.J.  Wellings, A Schedulability Compatible Multiprocessor Resource
 * Sharing Protocol - MrsP, Proceedings of the 25th Euromicro Conference on
 * Real-Time Systems (ECRTS 2013), July 2013.  It is a generalization of the
 * Priority Ceiling Protocol to SMP systems.  Each MrsP semaphore uses a
 * ceiling priority per scheduler instance.  A task obtaining or owning a MrsP
 * semaphore will execute with the ceiling priority for its scheduler instance
 * as specified by the MrsP semaphore object.  Tasks waiting to get ownership
 * of a MrsP semaphore will not relinquish the processor voluntarily.  In case
 * the owner of a MrsP semaphore gets preempted it can ask all tasks waiting
 * for this semaphore to help out and temporarily borrow the right to execute
 * on one of their assigned processors.
 *
 * @{
 */

/**
 * @brief MrsP status code.
 *
 * The values are chosen to directly map to RTEMS status codes.  In case this
 * implementation is used for other APIs, then for example the errno values can
 * be added with a bit shift.
 */
typedef enum {
  MRSP_SUCCESSFUL = 0,
  MRSP_TIMEOUT = 6,
  MRSP_INVALID_NUMBER = 10,
  MRSP_RESOUCE_IN_USE = 12,
  MRSP_UNSATISFIED = 13,
  MRSP_INCORRECT_STATE = 14,
  MRSP_INVALID_PRIORITY = 19,
  MRSP_NOT_OWNER_OF_RESOURCE = 23,
  MRSP_NO_MEMORY = 26
} MRSP_Status;

/**
 * @brief MrsP rival.
 *
 * The rivals are used by threads waiting for resource ownership.  They are
 * registered in the MRSP control block.
 */
typedef struct {
  /**
   * @brief The node for registration in the MRSP rival chain.
   *
   * @see MRSP_Control::Rivals.
   */
  Chain_Node Node;

  /**
   * @brief Identification of the rival thread.
   */
  Thread_Control *thread;

  /**
   * @brief The rival state.
   *
   * Initially no state bits are set (MRSP_RIVAL_STATE_WAITING).  The rival
   * will busy wait until a state change happens.  This can be
   * MRSP_RIVAL_STATE_NEW_OWNER or MRSP_RIVAL_STATE_TIMEOUT.
   */
  Atomic_Uint state;
} MRSP_Rival;

/**
 * @brief MrsP control block.
 */
typedef struct {
  /**
   * @brief Basic resource control.
   */
  Resource_Control Resource;

  /**
   * @brief A chain of MrsP rivals waiting for resource ownership.
   *
   * @see MRSP_Rival::Node.
   */
  Chain_Control Rivals;

  /**
   * @brief The initial priority of the owner before it was elevated to the
   * ceiling priority.
   */
  Priority_Control initial_priority_of_owner;

  /**
   * @brief One ceiling priority per scheduler instance.
   */
  Priority_Control *ceiling_priorities;
} MRSP_Control;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SMP */

#endif /* _RTEMS_SCORE_MRSP_H */
