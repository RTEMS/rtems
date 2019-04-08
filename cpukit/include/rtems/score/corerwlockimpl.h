/**
 * @file
 *
 * @ingroup RTEMSScoreRWLock
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

#include <rtems/score/percpu.h>
#include <rtems/score/status.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdog.h>

/**
 * @defgroup RTEMSScoreRWLock ScoreRWLock
 * 
 * @ingroup RTEMSScore
 * 
 * @brief ScoreRWLock
 */
/** @{ */

#ifdef __cplusplus
extern "C" {
#endif

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
 *  The following defines the control block used to manage each
 *  RWLock.
 */
typedef struct {
  /** This field is the Waiting Queue used to manage the set of tasks
   *  which are blocked waiting for the RWLock to be released.
   */
  Thread_queue_Syslock_queue Queue;

  /** This element is the current state of the RWLock.
   */
  CORE_RWLock_States current_state;

  /** This element contains the current number of thread waiting for this
   *  RWLock to be released. */
  unsigned int number_of_readers;
}   CORE_RWLock_Control;

/**
 * @brief Initializes a RWlock.
 *
 * This routine initializes the RWLock.
 *
 * @param[out] the_rwlock is the RWLock to initialize.
 */
void _CORE_RWLock_Initialize(
  CORE_RWLock_Control *the_rwlock
);

/**
 * @brief Destroys a RWlock.
 *
 * This routine destroys the RWLock.
 *
 * @param[out] the_rwlock is the RWLock to destroy.
 */
RTEMS_INLINE_ROUTINE void _CORE_RWLock_Destroy(
  CORE_RWLock_Control *the_rwlock
)
{
  (void) the_rwlock;
}

/**
 * @brief Acquires the RWlock.
 *
 * @param[in, out] the_rwlock The RWlock to acquire.
 * @param queue_context The thread queue context.
 *
 * @return The executing thread.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_CORE_RWLock_Acquire(
  CORE_RWLock_Control  *the_rwlock,
  Thread_queue_Context *queue_context
)
{
  ISR_Level       level;
  Thread_Control *executing;

  _Thread_queue_Context_ISR_disable( queue_context, level );
  _Thread_queue_Context_set_ISR_level( queue_context, level );
  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &the_rwlock->Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );

  return executing;
}

/**
 * @brief Releases the RWlock.
 *
 * @param[in, out] the_rwlock The RWlock to release.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_RWLock_Release(
  CORE_RWLock_Control  *the_rwlock,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_release(
    &the_rwlock->Queue.Queue,
    &queue_context->Lock_context.Lock_context
  );
}

/**
 * @brief Obtains RWLock for reading.
 *
 * This routine attempts to obtain the RWLock for read access.
 *
 * @param[in, out] the_rwlock is the RWLock to wait for
 * @param wait Indicates whether the calling thread is willing to wait.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The RWlock was successfully seized.
 * @retval STATUS_UNAVAILABLE The RWlock is currently locked for writing
 *          and the calling thread is not willing to wait.
 * @retval STATUS_TIMEOUT A timeout occured.
 */

Status_Control _CORE_RWLock_Seize_for_reading(
  CORE_RWLock_Control  *the_rwlock,
  bool                  wait,
  Thread_queue_Context *queue_context
);

/**
 * @brief Obtains RWLock for writing.
 *
 * This routine attempts to obtain the RWLock for write exclusive access.
 *
 * @param[in, out] the_rwlock The RWLock to wait for.
 * @param wait Indicates whether the calling thread is willing to wait.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The RWLock was successfully obtained for write
 *          exclusive access.
 * @retval STATUS_UNAVAILABLE The RWlock is currently locked and the calling
 *          thread is not willing to wait.
 * @retval STATUS_TIMEOUT A timeout occurred.
 */
Status_Control _CORE_RWLock_Seize_for_writing(
  CORE_RWLock_Control  *the_rwlock,
  bool                  wait,
  Thread_queue_Context *queue_context
);

/**
 * @brief Releases the RWLock.
 *
 * This routine manually releases @a the_rwlock.  All of the threads waiting
 * for the RWLock will be readied.
 *
 * @param[in, out] the_rwlock The RWLock to surrender.
 *
 * @return STATUS_SUCCESSFUL This method is always successful.
 */
Status_Control _CORE_RWLock_Surrender( CORE_RWLock_Control *the_rwlock );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
