/**
 * @file
 *
 * @ingroup RTEMSScoreBarrier
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

#define CORE_BARRIER_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

/**
 *  @brief Initializes the core barrier.
 *
 *  This routine initializes the barrier based on the parameters passed.
 *
 *  @param[out] the_barrier The barrier to initialize.
 *  @param[out] the_barrier_attributes The attributes which define the behavior of this instance.
 */
void _CORE_barrier_Initialize(
  CORE_barrier_Control       *the_barrier,
  CORE_barrier_Attributes    *the_barrier_attributes
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
 * @brief Flushes the barrier.
 *
 * @param[in, out] the_barrier The barrier to flush.
 * @param[out] filter The filter for flushing.
 * @param[out] queue_context The thread queue context.
 */
uint32_t _CORE_barrier_Do_flush(
  CORE_barrier_Control      *the_barrier,
  Thread_queue_Flush_filter  filter,
  Thread_queue_Context      *queue_context
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
  return _CORE_barrier_Do_flush(
    the_barrier,
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
  _CORE_barrier_Do_flush(
    the_barrier,
    _Thread_queue_Flush_status_object_was_deleted,
    queue_context
  );
}

/**
 * @brief Checks if the barrier is automatic.
 *
 * This function returns true if the automatic release attribute is
 * enabled in the @a attribute_set and false otherwise.
 *
 * @param the_attribute The attribute set to test.
 *
 * @retval true The automatic release attribute is enabled.
 * @retval false The automatic release attribute is not enabled.
 */
RTEMS_INLINE_ROUTINE bool _CORE_barrier_Is_automatic(
  CORE_barrier_Attributes *the_attribute
)
{
   return
     (the_attribute->discipline == CORE_BARRIER_AUTOMATIC_RELEASE);
}

/**
 * @brief Returns the number of currently waiting threads.
 *
 * This routine returns the number of threads currently waiting at the barrier.
 *
 * @param[in] the_barrier The barrier to obtain the number of blocked
 *            threads of.
 *
 * @return the current count of waiting threads of this barrier.
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
