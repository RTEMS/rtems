/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
static inline void _CORE_barrier_Destroy(
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
static inline void _CORE_barrier_Acquire_critical(
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
static inline void _CORE_barrier_Release(
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
static inline uint32_t _CORE_barrier_Surrender(
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
static inline void _CORE_barrier_Flush(
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

/**
 * @brief Gets the number of threads waiting at the barrier.
 *
 * This routine returns the count of threads currently waiting at the barrier.
 *
 * @param[in] the_barrier The barrier to check.
 * @param[in] queue_context The thread queue context.
 *
 * @return The number of threads currently waiting at the barrier.
 */
static inline uint32_t _CORE_barrier_Get_number_waiting(
  const CORE_barrier_Control *the_barrier
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
