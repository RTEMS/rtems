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
#include <rtems/score/status.h>
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
  CORE_RWLock_Control  *the_rwlock,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Acquire_critical(
    &the_rwlock->Wait_queue,
    &queue_context->Lock_context
  );
}

RTEMS_INLINE_ROUTINE void _CORE_RWLock_Release(
  CORE_RWLock_Control  *the_rwlock,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Release(
    &the_rwlock->Wait_queue,
    &queue_context->Lock_context
  );
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
 */

Status_Control _CORE_RWLock_Seize_for_reading(
  CORE_RWLock_Control  *the_rwlock,
  Thread_Control       *executing,
  bool                  wait,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
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
 */
Status_Control _CORE_RWLock_Seize_for_writing(
  CORE_RWLock_Control  *the_rwlock,
  Thread_Control       *executing,
  bool                  wait,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
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
Status_Control _CORE_RWLock_Surrender(
  CORE_RWLock_Control  *the_rwlock,
  Thread_queue_Context *queue_context
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
