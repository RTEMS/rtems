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

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Condition_variables_Control, Wait_queue );

static void _POSIX_Condition_variables_Enqueue_callout(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  POSIX_Condition_variables_Control *the_cond;
  int                                mutex_error;

  the_cond = POSIX_CONDITION_VARIABLE_OF_THREAD_QUEUE_QUEUE( queue );

  mutex_error = pthread_mutex_unlock( &the_cond->mutex );
  if ( mutex_error != 0 ) {
    /*
     *  Historically, we ignored the unlock status since the behavior
     *  is undefined by POSIX. But GNU/Linux returns EPERM in this
     *  case, so we follow their lead.
     */
    _Assert( mutex_error == EINVAL || mutex_error == EPERM );
    _Thread_queue_Extract( the_thread );
    the_thread->Wait.return_code= STATUS_NOT_OWNER;
  }
}

int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  const struct timespec     *abstime
)
{
  POSIX_Condition_variables_Control *the_cond;
  Thread_queue_Context               queue_context;
  int                                error;
  int                                mutex_error;
  Thread_Control                    *executing;
  Watchdog_Interval                  timeout;
  bool                               already_timedout;
  TOD_Absolute_timeout_conversion_results  status;

  if ( mutex == NULL ) {
    return EINVAL;
  }

  the_cond = _POSIX_Condition_variables_Get( cond, &queue_context );

  if ( the_cond == NULL ) {
    return EINVAL;
  }

  already_timedout = false;

  if ( abstime != NULL ) {
    /*
     *  POSIX requires that blocking calls with timeouts that take
     *  an absolute timeout must ignore issues with the absolute
     *  time provided if the operation would otherwise succeed.
     *  So we check the abstime provided, and hold on to whether it
     *  is valid or not.  If it isn't correct and in the future,
     *  then we do a polling operation and convert the UNSATISFIED
     *  status into the appropriate error.
     */
    _Assert( the_cond->clock );
    status = _TOD_Absolute_timeout_to_ticks(abstime, the_cond->clock, &timeout);
    if ( status == TOD_ABSOLUTE_TIMEOUT_INVALID )
      return EINVAL;

    if ( status == TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST ||
        status == TOD_ABSOLUTE_TIMEOUT_IS_NOW ) {
      already_timedout = true;
    } else {
      _Thread_queue_Context_set_relative_timeout( &queue_context, timeout );
    }
  } else {
    _Thread_queue_Context_set_no_timeout( &queue_context );
  }

  _POSIX_Condition_variables_Acquire_critical( the_cond, &queue_context );

  if (
    the_cond->mutex != POSIX_CONDITION_VARIABLES_NO_MUTEX
      && the_cond->mutex != *mutex
  ) {
    _POSIX_Condition_variables_Release( the_cond, &queue_context );
    return EINVAL;
  }

  the_cond->mutex = *mutex;
  executing = _Thread_Executing;

  if ( !already_timedout ) {
    _Thread_queue_Context_set_thread_state(
      &queue_context,
      STATES_WAITING_FOR_CONDITION_VARIABLE
    );
    _Thread_queue_Context_set_enqueue_callout(
      &queue_context,
      _POSIX_Condition_variables_Enqueue_callout
    );
    _Thread_queue_Enqueue(
      &the_cond->Wait_queue.Queue,
      POSIX_CONDITION_VARIABLES_TQ_OPERATIONS,
      executing,
      &queue_context
    );
    error = _POSIX_Get_error_after_wait( executing );
  } else {
    _POSIX_Condition_variables_Release( the_cond, &queue_context );

    mutex_error = pthread_mutex_unlock( &the_cond->mutex );
    if ( mutex_error != 0 ) {
      error = EPERM;
    } else {
      error = ETIMEDOUT;
    }
  }

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
    mutex_error = pthread_mutex_lock( mutex );
    if ( mutex_error != 0 ) {
      _Assert( mutex_error == EINVAL );
      error = EINVAL;
    }
  }

  return error;
}
