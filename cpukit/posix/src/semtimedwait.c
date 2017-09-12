/**
 * @file
 *
 * @brief Lock a Semaphore
 * @ingroup POSIX_SEMAPHORE POSIX Semaphores Support
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/semaphoreimpl.h>
#include <rtems/score/todimpl.h>

/*
 *  11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 *  NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */

int sem_timedwait(
  sem_t                 *__restrict _sem,
  const struct timespec *__restrict abstime
)
{
  Sem_Control          *sem;
  Thread_queue_Context  queue_context;
  ISR_Level             level;
  Thread_Control       *executing;
  unsigned int          count;

  POSIX_SEMAPHORE_VALIDATE_OBJECT( _sem );

  sem = _Sem_Get( &_sem->_Semaphore );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Sem_Queue_acquire_critical( sem, &queue_context );

  count = sem->count;
  if ( __predict_true( count > 0 ) ) {
    sem->count = count - 1;
    _Sem_Queue_release( sem, level, &queue_context );
    return 0;
  } else {
    Watchdog_Interval ticks;
    Status_Control    status;

    switch ( _TOD_Absolute_timeout_to_ticks( abstime, CLOCK_REALTIME, &ticks ) ) {
      case TOD_ABSOLUTE_TIMEOUT_INVALID:
        _Sem_Queue_release( sem, level, &queue_context );
        rtems_set_errno_and_return_minus_one( EINVAL );
        break;
      case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
      case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
        _Sem_Queue_release( sem, level, &queue_context );
        rtems_set_errno_and_return_minus_one( ETIMEDOUT );
        break;
      default:
        break;
    }

    _Thread_queue_Context_set_thread_state(
      &queue_context,
      STATES_WAITING_FOR_SEMAPHORE
    );
    _Thread_queue_Context_set_do_nothing_enqueue_callout( &queue_context );
    _Thread_queue_Context_set_relative_timeout( &queue_context, ticks );
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Enqueue(
      &sem->Queue.Queue,
      SEMAPHORE_TQ_OPERATIONS,
      executing,
      &queue_context
    );
    status = _Thread_Wait_get_status( executing );
    return _POSIX_Zero_or_minus_one_plus_errno( status );
  }
}
