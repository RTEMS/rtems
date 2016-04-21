/**
 * @file
 *
 * @brief Inlined Routines Associated with the SuperCore RWLock
 *
 * This include file contains all of the inlined routines associated
 * with the SuperCore RWLock.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORERWLOCKIMPL_H
#define _RTEMS_SCORE_CORERWLOCKIMPL_H

#include <rtems/score/corerwlock.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreRWLock
 */
/**@{**/

#define CORE_RWLOCK_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

/**
 *  Core RWLock handler return statuses.
 */
typedef enum {
  /** This status indicates that the operation completed successfully. */
  CORE_RWLOCK_SUCCESSFUL,
  /** This status indicates that the thread was blocked waiting for an */
  CORE_RWLOCK_WAS_DELETED,
  /** This status indicates that the rwlock was not immediately available. */
  CORE_RWLOCK_UNAVAILABLE,
  /** This status indicates that the calling task was willing to block
   *  but the operation was unable to complete within the time allotted
   *  because the resource never became available.
   */
  CORE_RWLOCK_TIMEOUT
}   CORE_RWLock_Status;

/** This is the last status value.
 */
#define CORE_RWLOCK_STATUS_LAST CORE_RWLOCK_TIMEOUT

/**
 *  This is used to denote that a thread is blocking waiting for
 *  read-only access to the RWLock.
 */
#define CORE_RWLOCK_THREAD_WAITING_FOR_READ  0

/**
 *  This is used to denote that a thread is blocking waiting for
 *  write-exclusive access to the RWLock.
 */
#define CORE_RWLOCK_THREAD_WAITING_FOR_WRITE 1

/**
 *  @brief Initialize a RWlock.
 *
 *  This routine initializes the RWLock based on the parameters passed.
 *
 *  @param[in] the_rwlock is the RWLock to initialize
 */
void _CORE_RWLock_Initialize(
  CORE_RWLock_Control *the_rwlock
);

RTEMS_INLINE_ROUTINE void _CORE_RWLock_Destroy(
  CORE_RWLock_Control *the_rwlock
)
{
  _Thread_queue_Destroy( &the_rwlock->Wait_queue );
}

RTEMS_INLINE_ROUTINE void _CORE_RWLock_Acquire_critical(
  CORE_RWLock_Control *the_rwlock,
  ISR_lock_Context    *lock_context
)
{
  _Thread_queue_Acquire_critical( &the_rwlock->Wait_queue, lock_context );
}

RTEMS_INLINE_ROUTINE void _CORE_RWLock_Release(
  CORE_RWLock_Control *the_rwlock,
  ISR_lock_Context    *lock_context
)
{
  _Thread_queue_Release( &the_rwlock->Wait_queue, lock_context );
}

/**
 *  @brief Obtain RWLock for reading.
 *
 *  This routine attempts to obtain the RWLock for read access.
 *
 *  @param[in] the_rwlock is the RWLock to wait for
 *  @param[in] wait is true if the calling thread is willing to wait
 *  @param[in] timeout is the number of ticks the calling thread is willing
 *         to wait if @a wait is true.
 *
 * @note Status is returned via the thread control block.
 */

void _CORE_RWLock_Seize_for_reading(
  CORE_RWLock_Control *the_rwlock,
  Thread_Control      *executing,
  bool                 wait,
  Watchdog_Interval    timeout,
  ISR_lock_Context    *lock_context
);

/**
 *  @brief Obtain RWLock for writing.
 *
 *  This routine attempts to obtain the RWLock for write exclusive access.
 *
 *  @param[in] the_rwlock is the RWLock to wait for
 *  @param[in] wait is true if the calling thread is willing to wait
 *  @param[in] timeout is the number of ticks the calling thread is willing
 *         to wait if @a wait is true.
 *
 * @note Status is returned via the thread control block.
 */
void _CORE_RWLock_Seize_for_writing(
  CORE_RWLock_Control *the_rwlock,
  Thread_Control      *executing,
  bool                 wait,
  Watchdog_Interval    timeout,
  ISR_lock_Context    *lock_context
);

/**
 *  @brief Release the RWLock.
 *
 *  This routine manually releases @a the_rwlock.  All of the threads waiting
 *  for the RWLock will be readied.
 *
 *  @param[in] the_rwlock is the RWLock to surrender
 *
 *  @retval Status is returned to indicate successful or failure.
 */
CORE_RWLock_Status _CORE_RWLock_Surrender(
  CORE_RWLock_Control *the_rwlock,
  ISR_lock_Context    *lock_context
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
