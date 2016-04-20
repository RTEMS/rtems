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
#include <rtems/score/threadqimpl.h>

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

#define CORE_BARRIER_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

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

RTEMS_INLINE_ROUTINE void _CORE_barrier_Acquire_critical(
  CORE_barrier_Control *the_barrier,
  ISR_lock_Context     *lock_context
)
{
  _Thread_queue_Acquire_critical( &the_barrier->Wait_queue, lock_context );
}

RTEMS_INLINE_ROUTINE void _CORE_barrier_Release(
  CORE_barrier_Control *the_barrier,
  ISR_lock_Context     *lock_context
)
{
  _Thread_queue_Release( &the_barrier->Wait_queue, lock_context );
}

void _CORE_barrier_Do_seize(
  CORE_barrier_Control    *the_barrier,
  Thread_Control          *executing,
  bool                     wait,
  Watchdog_Interval        timeout,
#if defined(RTEMS_MULTIPROCESSING)
  Thread_queue_MP_callout  mp_callout,
  Objects_Id               mp_id,
#endif
  ISR_lock_Context        *lock_context
);

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
 *  @param[in] wait is true if the calling thread is willing to wait
 *  @param[in] timeout is the number of ticks the calling thread is willing
 *         to wait if @a wait is true.
 *  @param[in] mp_callout is the routine to invoke if the
 *         thread unblocked is remote
 *  @param[in] mp_id is the id of the object being waited upon
 *
 * @note Status is returned via the thread control block.
 */
#if defined(RTEMS_MULTIPROCESSING)
  #define _CORE_barrier_Seize( \
    the_barrier, \
    executing, \
    wait, \
    timeout, \
    mp_callout, \
    mp_id, \
    lock_context \
  ) \
    _CORE_barrier_Do_seize( \
      the_barrier, \
      executing, \
      wait, \
      timeout, \
      mp_callout, \
      mp_id, \
      lock_context \
    )
#else
  #define _CORE_barrier_Seize( \
    the_barrier, \
    executing, \
    wait, \
    timeout, \
    mp_callout, \
    mp_id, \
    lock_context \
  ) \
    _CORE_barrier_Do_seize( \
      the_barrier, \
      executing, \
      wait, \
      timeout, \
      lock_context \
    )
#endif

uint32_t _CORE_barrier_Do_surrender(
  CORE_barrier_Control      *the_barrier,
  Thread_queue_Flush_filter  filter,
#if defined(RTEMS_MULTIPROCESSING)
  Thread_queue_MP_callout    mp_callout,
  Objects_Id                 mp_id,
#endif
  ISR_lock_Context          *lock_context
);

/**
 *  @brief Manually release the barrier.
 *
 *  This routine manually releases the barrier.  All of the threads waiting
 *  for the barrier will be readied.
 *
 *  @param[in] the_barrier is the barrier to surrender
 *  @param[in] mp_callout is the routine to invoke if the
 *         thread unblocked is remote
 *  @param[in] mp_id is the id of the object for a remote unblock
 *
 *  @retval the number of unblocked threads
 */
#if defined(RTEMS_MULTIPROCESSING)
  #define _CORE_barrier_Surrender( \
    the_barrier, \
    mp_callout, \
    mp_id, \
    lock_context \
  ) \
    _CORE_barrier_Do_surrender( \
      the_barrier, \
      mp_callout, \
      mp_id, \
      _Thread_queue_Flush_default_filter, \
      lock_context \
    )
#else
  #define _CORE_barrier_Surrender( \
    the_barrier, \
    mp_callout, \
    mp_id, \
    lock_context \
  ) \
    _CORE_barrier_Do_surrender( \
      the_barrier, \
      _Thread_queue_Flush_default_filter, \
      lock_context \
    )
#endif

Thread_Control *_CORE_barrier_Was_deleted(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue,
  ISR_lock_Context   *lock_context
);

/* Must be a macro due to the multiprocessing dependent parameters */
#if defined(RTEMS_MULTIPROCESSING)
  #define _CORE_barrier_Flush( \
    the_barrier, \
    mp_callout, \
    mp_id, \
    lock_context \
  ) \
    _CORE_barrier_Do_surrender( \
      the_barrier, \
      _CORE_barrier_Was_deleted, \
      mp_callout, \
      mp_id, \
      lock_context \
    )
#else
  #define _CORE_barrier_Flush( \
    the_barrier, \
    mp_callout, \
    mp_id, \
    lock_context \
  ) \
    _CORE_barrier_Do_surrender( \
      the_barrier, \
      _CORE_barrier_Was_deleted, \
      lock_context \
    )
#endif

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
