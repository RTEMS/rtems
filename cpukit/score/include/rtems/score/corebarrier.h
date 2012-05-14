/**
 *  @file  rtems/score/corebarrier.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Barrier Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREBARRIER_H
#define _RTEMS_SCORE_COREBARRIER_H

/**
 *  @defgroup ScoreBarrier Barrier Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Barrier services used in all of the APIs supported by RTEMS.
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
 *  to support global/multiprocessor operations on barriers.
 */
typedef void ( *CORE_barrier_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/**
 *  Flavors of barriers.
 */
typedef enum {
  /** This specifies that the barrier will automatically release when
   *  the user specified number of threads have arrived at the barrier.
   */
  CORE_BARRIER_AUTOMATIC_RELEASE,
  /** This specifies that the user will have to manually release the barrier
   *  in order to release the waiting threads.
   */
  CORE_BARRIER_MANUAL_RELEASE
}   CORE_barrier_Disciplines;

/**
 *  Core Barrier handler return statuses.
 */
typedef enum {
  /** This status indicates that the operation completed successfully. */
  CORE_BARRIER_STATUS_SUCCESSFUL,
  /** This status indicates that the barrier is configured for automatic
   *  release and the caller tripped the automatic release.  The caller
   *  thus did not block.
   */
  CORE_BARRIER_STATUS_AUTOMATICALLY_RELEASED,
  /** This status indicates that the thread was blocked waiting for an
   *  operation to complete and the barrier was deleted.
   */
  CORE_BARRIER_WAS_DELETED,
  /** This status indicates that the calling task was willing to block
   *  but the operation was unable to complete within the time allotted
   *  because the resource never became available.
   */
  CORE_BARRIER_TIMEOUT
}   CORE_barrier_Status;

/**
 *  @brief Core Barrier Last Status
 *
 *  This is the last status value.
 */
#define CORE_BARRIER_STATUS_LAST CORE_BARRIER_TIMEOUT

/**
 *  The following defines the control block used to manage the
 *  attributes of each barrier.
 */
typedef struct {
  /** This field indicates whether the barrier is automatic or manual.
   */
  CORE_barrier_Disciplines  discipline;
  /** This element indicates the number of threads which must arrive at the
   *  barrier to trip the automatic release.
   */
  uint32_t                  maximum_count;
}   CORE_barrier_Attributes;

/**
 *  The following defines the control block used to manage each
 *  barrier.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting for the barrier to be released.
   */
  Thread_queue_Control     Wait_queue;
  /** This element is the set of attributes which define this instance's
   *  behavior.
   */
  CORE_barrier_Attributes  Attributes;
  /** This element contains the current number of thread waiting for this
   *  barrier to be released. */
  uint32_t                 number_of_waiting_threads;
}   CORE_barrier_Control;

/**
 *  This routine initializes the barrier based on the parameters passed.
 *
 *  @param[in] the_barrier is the barrier to initialize
 *  @param[in] the_barrier_attributes define the behavior of this instance
 */
void _CORE_barrier_Initialize(
  CORE_barrier_Control       *the_barrier,
  CORE_barrier_Attributes    *the_barrier_attributes
);

/**
 *  This routine wait for the barrier to be released.  If the barrier
 *  is set to automatic and this is the appropriate thread, then it returns
 *  immediately.  Otherwise, the calling thread is blocked until the barrier
 *  is released.
 *
 *  @param[in] the_barrier is the barrier to wait for
 *  @param[in] id is the id of the object being waited upon
 *  @param[in] wait is true if the calling thread is willing to wait
 *  @param[in] timeout is the number of ticks the calling thread is willing
 *         to wait if @a wait is true.
 *  @param[in] api_barrier_mp_support is the routine to invoke if the
 *         thread unblocked is remote
 *
 * @note Status is returned via the thread control block.
 */
void _CORE_barrier_Wait(
  CORE_barrier_Control                *the_barrier,
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_barrier_API_mp_support_callout  api_barrier_mp_support
);

/**
 *  This routine manually releases the barrier.  All of the threads waiting
 *  for the barrier will be readied.
 *
 *  @param[in] the_barrier is the barrier to surrender
 *  @param[in] id is the id of the object for a remote unblock
 *  @param[in] api_barrier_mp_support is the routine to invoke if the
 *         thread unblocked is remote
 *
 *  @return the number of unblocked threads
 */
uint32_t _CORE_barrier_Release(
  CORE_barrier_Control                *the_barrier,
  Objects_Id                           id,
  CORE_barrier_API_mp_support_callout  api_barrier_mp_support
);

/**
 *  This routine assists in the deletion of a barrier by flushing the
 *  associated wait queue.
 *
 *  @param[in] _the_barrier is the barrier to flush
 *  @param[in] _remote_extract_callout is the routine to invoke if the
 *         thread unblocked is remote
 *  @param[in] _status is the status to be returned to the unblocked thread
 */
#define _CORE_barrier_Flush( _the_barrier, _remote_extract_callout, _status) \
  _Thread_queue_Flush( \
    &((_the_barrier)->Wait_queue), \
    (_remote_extract_callout), \
    (_status) \
  )

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/corebarrier.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */
