/**
 * @file
 *
 * @ingroup RTEMSScoreBarrier
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreBarrier which are only used by the implementation.
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
#include <rtems/score/status.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreBarrier
 *
 * @{
 */

/**
 * @brief This maximum thread count constant indicates that the barrier is a
 *   manual release barrier.
 */
#define CORE_BARRIER_MANUAL_RELEASE_MAXIMUM_COUNT 0

/**
 * @brief These thread queue operations are used for core barriers.
 *
 * They are a specialization of ::_Thread_queue_Operations_FIFO.  The only
 * difference is that the extract operation decrements
 * CORE_barrier_Control::number_of_waiting_threads.
 */
extern const Thread_queue_Operations _CORE_barrier_Thread_queue_operations;

/**
 * @brief Initializes the core barrier.
 *
 * @param[out] the_barrier is the barrier to initialize.
 *
 * @param maximum_count is the number of threads which must arrive at the
 *   barrier to trip the automatic release or
 *   ::CORE_BARRIER_MANUAL_RELEASE_MAXIMUM_COUNT to indicate a manual release
 *   barrier.
 */
void _CORE_barrier_Initialize(
  CORE_barrier_Control *the_barrier,
  uint32_t              maximum_count
);

/**
 * @brief Destroys the core barrier.
 *
 * This routine destroys the barrier.
 *
 * @param[out] the_barrier The barrier to destroy.
 */
RTEMS_INLINE_ROUTINE void _CORE_barrier_Destroy(
  CORE_barrier_Control *the_barrier
)
{
  _Thread_queue_Destroy( &the_barrier->Wait_queue );
}

/**
 * @brief Acquires critical core barrier.
 *
 * @param[in, out] the_barrier The barrier to acquire.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_barrier_Acquire_critical(
  CORE_barrier_Control *the_barrier,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Acquire_critical( &the_barrier->Wait_queue, queue_context );
}

/**
 * @brief Releases core barrier.
 *
 * @param[in, out] the_barrier The barrier to release.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_barrier_Release(
  CORE_barrier_Control *the_barrier,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Release( &the_barrier->Wait_queue, queue_context );
}

/**
 * @brief Waits for the barrier.
 *
 * This routine waits for the barrier to be released.  If the barrier
 * is set to automatic and this is the appropriate thread, then it returns
 * immediately.  Otherwise, the calling thread is blocked until the barrier
 * is released.
 *
 * @param[in, out] the_barrier The barrier to wait for.
 * @param[in, out] executing The currently executing thread.
 * @param wait This parameter is true if the calling thread is willing to wait.
 * @param queue_context The thread queue context.
 *
 * @return The method status.
 */
Status_Control _CORE_barrier_Seize(
  CORE_barrier_Control *the_barrier,
  Thread_Control       *executing,
  bool                  wait,
  Thread_queue_Context *queue_context
);

/**
 * @brief Manually releases the barrier.
 *
 * This routine manually releases the barrier.  All of the threads waiting
 * for the barrier will be readied.
 *
 * @param[in, out] the_barrier The barrier to surrender.
 * @param[out] queue_context The thread queue context.
 *
 * @return The number of unblocked threads.
 */
RTEMS_INLINE_ROUTINE uint32_t _CORE_barrier_Surrender(
  CORE_barrier_Control *the_barrier,
  Thread_queue_Context *queue_context
)
{
  return _Thread_queue_Flush_critical(
    &the_barrier->Wait_queue.Queue,
    &_CORE_barrier_Thread_queue_operations,
    _Thread_queue_Flush_default_filter,
    queue_context
  );
}

/**
 * @brief Flushes the barrier using _CORE_barrier_Do_flush().
 *
 * @param[in, out] the_barrier The barrier to flush.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_barrier_Flush(
  CORE_barrier_Control *the_barrier,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Flush_critical(
    &the_barrier->Wait_queue.Queue,
    &_CORE_barrier_Thread_queue_operations,
    _Thread_queue_Flush_status_object_was_deleted,
    queue_context
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
