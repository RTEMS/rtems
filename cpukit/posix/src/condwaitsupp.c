/**
 * @file
 *
 * @brief POSIX Condition Variables Wait Support
 * @ingroup POSIX_COND_VARS
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/condimpl.h>
#include <rtems/posix/posixapi.h>
#include <rtems/score/assert.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/threaddispatch.h>

static void _POSIX_Condition_variables_Mutex_unlock(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  POSIX_Condition_variables_Control *the_cond;
  int                                mutex_error;

  the_cond = POSIX_CONDITION_VARIABLE_OF_THREAD_QUEUE_QUEUE( queue );

  mutex_error = pthread_mutex_unlock( the_cond->mutex );
  if ( mutex_error != 0 ) {
    /*
     *  Historically, we ignored the unlock status since the behavior
     *  is undefined by POSIX. But GNU/Linux returns EPERM in this
     *  case, so we follow their lead.
     */
    _Assert( mutex_error == EINVAL || mutex_error == EPERM );
    _Thread_Continue( the_thread, STATUS_NOT_OWNER );
  }
}

static void _POSIX_Condition_variables_Enqueue_no_timeout(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  _POSIX_Condition_variables_Mutex_unlock( queue, the_thread, queue_context );
}

static void _POSIX_Condition_variables_Enqueue_with_timeout_monotonic(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Add_timeout_monotonic_timespec(
    queue,
    the_thread,
    cpu_self,
    queue_context
  );
  _POSIX_Condition_variables_Mutex_unlock( queue, the_thread, queue_context );
}

static void _POSIX_Condition_variables_Enqueue_with_timeout_realtime(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Add_timeout_realtime_timespec(
    queue,
    the_thread,
    cpu_self,
    queue_context
  );
  _POSIX_Condition_variables_Mutex_unlock( queue, the_thread, queue_context );
}

int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  const struct timespec     *abstime
)
{
  POSIX_Condition_variables_Control *the_cond;
  unsigned long                      flags;
  Thread_queue_Context               queue_context;
  int                                error;
  Thread_Control                    *executing;

  the_cond = _POSIX_Condition_variables_Get( cond );
  POSIX_CONDITION_VARIABLES_VALIDATE_OBJECT( the_cond, flags );

  _Thread_queue_Context_initialize( &queue_context );

  if ( abstime != NULL ) {
    _Thread_queue_Context_set_timeout_argument( &queue_context, abstime );

    if ( _POSIX_Condition_variables_Get_clock( flags ) == CLOCK_MONOTONIC ) {
      _Thread_queue_Context_set_enqueue_callout(
        &queue_context,
        _POSIX_Condition_variables_Enqueue_with_timeout_monotonic
      );
    } else {
      _Thread_queue_Context_set_enqueue_callout(
        &queue_context,
        _POSIX_Condition_variables_Enqueue_with_timeout_realtime
      );
    }
  } else {
    _Thread_queue_Context_set_enqueue_callout(
      &queue_context,
      _POSIX_Condition_variables_Enqueue_no_timeout
    );
  }

  executing = _POSIX_Condition_variables_Acquire( the_cond, &queue_context );

  if (
    the_cond->mutex != POSIX_CONDITION_VARIABLES_NO_MUTEX
      && the_cond->mutex != mutex
  ) {
    _POSIX_Condition_variables_Release( the_cond, &queue_context );
    return EINVAL;
  }

  the_cond->mutex = mutex;

  _Thread_queue_Context_set_thread_state(
    &queue_context,
    STATES_WAITING_FOR_CONDITION_VARIABLE
  );
  _Thread_queue_Enqueue(
    &the_cond->Queue.Queue,
    POSIX_CONDITION_VARIABLES_TQ_OPERATIONS,
    executing,
    &queue_context
  );
  error = _POSIX_Get_error_after_wait( executing );

  /*
   *  If the thread is interrupted, while in the thread queue, by
   *  a POSIX signal, then pthread_cond_wait returns spuriously,
   *  according to the POSIX standard. It means that pthread_cond_wait
   *  returns a success status, except for the fact that it was not
   *  woken up a pthread_cond_signal() or a pthread_cond_broadcast().
   */

  if ( error == EINTR ) {
    error = 0;
  }

  /*
   *  When we get here the dispatch disable level is 0.
   */

  if ( error != EPERM ) {
    int mutex_error;

    mutex_error = pthread_mutex_lock( mutex );
    if ( mutex_error != 0 ) {
      _Assert( mutex_error == EINVAL );
      error = EINVAL;
    }
  }

  return error;
}
