/**
 *  @file  rtems/score/corerwlock.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORERWLOCK_H
#define _RTEMS_SCORE_CORERWLOCK_H

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

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>

/**
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on RWLocks.
 */
typedef void ( *CORE_RWLock_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

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

/**
 *  This routine initializes the RWLock based on the parameters passed.
 *
 *  @param[in] the_rwlock is the RWLock to initialize
 *  @param[in] the_rwlock_attributes define the behavior of this instance
 */
void _CORE_RWLock_Initialize(
  CORE_RWLock_Control       *the_rwlock,
  CORE_RWLock_Attributes    *the_rwlock_attributes
);

/**
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
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_RWLock_API_mp_support_callout   api_rwlock_mp_support
);

/**
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
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_RWLock_API_mp_support_callout   api_rwlock_mp_support
);

/**
 *  This routine manually releases the RWLock.  All of the threads waiting
 *  for the RWLock will be readied.
 *
 *  @param[in] the_rwlock is the RWLock to surrender
 *
 *  @return Status is returned to indicate successful or failure.
 */
CORE_RWLock_Status _CORE_RWLock_Release(
  CORE_RWLock_Control                *the_rwlock
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
 *  @brief RWLock Specific Thread Queue Timeout
 *
 *  This routine processes a thread which timeouts while waiting on
 *  an RWLock's thread queue. It is called by the watchdog handler.
 *
 *  @param[in] id is the Id of thread to timeout
 *  @param[in] ignored is an unused pointer to a caller defined area
 */

void _CORE_RWLock_Timeout(
  Objects_Id  id,
  void       *ignored
);


#ifndef __RTEMS_APPLICATION__
#include <rtems/score/corerwlock.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
