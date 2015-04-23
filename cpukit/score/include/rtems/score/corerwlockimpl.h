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

/**
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on RWLocks.
 */
typedef void ( *CORE_RWLock_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

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
 *  @param[in] the_rwlock_attributes define the behavior of this instance
 */
void _CORE_RWLock_Initialize(
  CORE_RWLock_Control       *the_rwlock,
  CORE_RWLock_Attributes    *the_rwlock_attributes
);

RTEMS_INLINE_ROUTINE void _CORE_RWLock_Destroy(
  CORE_RWLock_Control *the_rwlock
)
{
  _Thread_queue_Destroy( &the_rwlock->Wait_queue );
}

/**
 *  @brief Obtain RWLock for reading.
 *
 *  This routine attempts to obtain the RWLock for read access.
 *
 *  @param[in] the_rwlock is the RWLock to wait for
 *  @param[in] id is the id of the object being waited upon
 *  @param[in] wait is true if the calling thread is willing to wait
 *  @param[in] timeout is the number of ticks the calling thread is willing
 *         to wait if @a wait is true.
 *  @param[in] api_rwlock_mp_support is the routine to invoke if the
 *         thread unblocked is remote
 *
 * @note Status is returned via the thread control block.
 */

void _CORE_RWLock_Obtain_for_reading(
  CORE_RWLock_Control                 *the_rwlock,
  Thread_Control                      *executing,
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_RWLock_API_mp_support_callout   api_rwlock_mp_support
);

/**
 *  @brief Obtain RWLock for writing.
 *
 *  This routine attempts to obtain the RWLock for write exclusive access.
 *
 *  @param[in] the_rwlock is the RWLock to wait for
 *  @param[in] id is the id of the object being waited upon
 *  @param[in] wait is true if the calling thread is willing to wait
 *  @param[in] timeout is the number of ticks the calling thread is willing
 *         to wait if @a wait is true.
 *  @param[in] api_rwlock_mp_support is the routine to invoke if the
 *         thread unblocked is remote
 *
 * @note Status is returned via the thread control block.
 */
void _CORE_RWLock_Obtain_for_writing(
  CORE_RWLock_Control                 *the_rwlock,
  Thread_Control                      *executing,
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_RWLock_API_mp_support_callout   api_rwlock_mp_support
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
CORE_RWLock_Status _CORE_RWLock_Release(
  CORE_RWLock_Control *the_rwlock,
  Thread_Control      *executing
);

/**
 *  This routine assists in the deletion of a RWLock by flushing the
 *  associated wait queue.
 *
 *  @param[in] _the_rwlock is the RWLock to flush
 *  @param[in] _remote_extract_callout is the routine to invoke if the
 *         thread unblocked is remote
 *  @param[in] _status is the status to be returned to the unblocked thread
 */
#define _CORE_RWLock_Flush( _the_rwlock, _remote_extract_callout, _status) \
  _Thread_queue_Flush( \
    &((_the_rwlock)->Wait_queue), \
    (_remote_extract_callout), \
    (_status) \
  )

/**
 * This method is used to initialize core rwlock attributes.
 *
 * @param[in] the_attributes pointer to the attributes to initialize.
 */
RTEMS_INLINE_ROUTINE void _CORE_RWLock_Initialize_attributes(
  CORE_RWLock_Attributes *the_attributes
)
{
  the_attributes->XXX = 0;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
