/**
 * @file
 *
 * @brief Inlined Routines Associated with the SuperCore Barrier
 *
 * This include file contains all of the inlined routines associated
 * with the SuperCore barrier.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREBARRIERIMPL_H
#define _RTEMS_SCORE_COREBARRIERIMPL_H

#include <rtems/score/corebarrier.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreBarrier
 */
/**@{**/

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
 *  @brief Core barrier last status value.
 *
 *  This is the last status value.
 */
#define CORE_BARRIER_STATUS_LAST CORE_BARRIER_TIMEOUT

/**
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on barriers.
 */
typedef void ( *CORE_barrier_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/**
 *  @brief Initialize core barrier.
 *
 *  This routine initializes the barrier based on the parameters passed.
 *
 *  @param[in] the_barrier is the barrier to initialize
 *  @param[in] the_barrier_attributes define the behavior of this instance
 */
void _CORE_barrier_Initialize(
  CORE_barrier_Control       *the_barrier,
  CORE_barrier_Attributes    *the_barrier_attributes
);

RTEMS_INLINE_ROUTINE void _CORE_barrier_Destroy(
  CORE_barrier_Control *the_barrier
)
{
  _Thread_queue_Destroy( &the_barrier->Wait_queue );
}

/**
 *  @brief Wait for the barrier.
 *
 *  This routine wait for the barrier to be released.  If the barrier
 *  is set to automatic and this is the appropriate thread, then it returns
 *  immediately.  Otherwise, the calling thread is blocked until the barrier
 *  is released.
 *
 *  @param[in] the_barrier is the barrier to wait for
 *  @param[in,out] executing The currently executing thread.
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
  Thread_Control                      *executing,
  Objects_Id                           id,
  bool                                 wait,
  Watchdog_Interval                    timeout,
  CORE_barrier_API_mp_support_callout  api_barrier_mp_support
);

/**
 *  @brief Manually release the barrier.
 *
 *  This routine manually releases the barrier.  All of the threads waiting
 *  for the barrier will be readied.
 *
 *  @param[in] the_barrier is the barrier to surrender
 *  @param[in] id is the id of the object for a remote unblock
 *  @param[in] api_barrier_mp_support is the routine to invoke if the
 *         thread unblocked is remote
 *
 *  @retval the number of unblocked threads
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

/**
 * This function returns true if the automatic release attribute is
 * enabled in the @a attribute_set and false otherwise.
 *
 * @param[in] the_attribute is the attribute set to test
 *
 * @return true if the priority attribute is enabled
 */
RTEMS_INLINE_ROUTINE bool _CORE_barrier_Is_automatic(
  CORE_barrier_Attributes *the_attribute
)
{
   return
     (the_attribute->discipline == CORE_BARRIER_AUTOMATIC_RELEASE);
}

/**
 * This routine returns the number of threads currently waiting at the barrier.
 *
 * @param[in] the_barrier is the barrier to obtain the number of blocked
 *            threads for
 * @return the current count of this barrier
 */
RTEMS_INLINE_ROUTINE uint32_t  _CORE_barrier_Get_number_of_waiting_threads(
  CORE_barrier_Control  *the_barrier
)
{
  return the_barrier->number_of_waiting_threads;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
