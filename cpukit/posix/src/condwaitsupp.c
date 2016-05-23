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
#include <rtems/posix/muteximpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Condition_variables_Control, Wait_queue );

int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  Watchdog_Interval          timeout,
  bool                       already_timedout
)
{
  POSIX_Condition_variables_Control *the_cond;
  POSIX_Mutex_Control               *the_mutex;
  ISR_lock_Context                   lock_context;
  int                                status;
  int                                mutex_status;
  CORE_mutex_Status                  core_mutex_status;
  Per_CPU_Control                   *cpu_self;
  Thread_Control                    *executing;

  if ( mutex == NULL ) {
    return EINVAL;
  }

  the_cond = _POSIX_Condition_variables_Get( cond, &lock_context );

  if ( the_cond == NULL ) {
    return EINVAL;
  }

  _POSIX_Condition_variables_Acquire_critical( the_cond, &lock_context );

  if (
    the_cond->mutex != POSIX_CONDITION_VARIABLES_NO_MUTEX
      && the_cond->mutex != *mutex
  ) {
    _POSIX_Condition_variables_Release( the_cond, &lock_context );
    return EINVAL;
  }

  the_cond->mutex = *mutex;

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  executing = _Per_CPU_Get_executing( cpu_self );

  /*
   *  Historically, we ignored the unlock status since the behavior
   *  is undefined by POSIX. But GNU/Linux returns EPERM in this
   *  case, so we follow their lead.
   */

  the_mutex = _POSIX_Mutex_Get_no_protection( mutex );
  if (
    the_mutex == NULL
      || !_CORE_mutex_Is_owner( &the_mutex->Mutex, executing )
  ) {
    _POSIX_Condition_variables_Release( the_cond, &lock_context );
    _Thread_Dispatch_enable( cpu_self );
    return EPERM;
  }

  if ( !already_timedout ) {
    executing->Wait.return_code = 0;
    _Thread_queue_Enqueue_critical(
      &the_cond->Wait_queue.Queue,
      POSIX_CONDITION_VARIABLES_TQ_OPERATIONS,
      executing,
      STATES_WAITING_FOR_CONDITION_VARIABLE,
      timeout,
      ETIMEDOUT,
      &lock_context
    );
  } else {
    _POSIX_Condition_variables_Release( the_cond, &lock_context );
    executing->Wait.return_code = ETIMEDOUT;
  }

  _ISR_lock_ISR_disable( &lock_context );
  core_mutex_status = _CORE_mutex_Surrender(
    &the_mutex->Mutex,
    NULL,
    0,
    &lock_context
  );
  _Assert( core_mutex_status == CORE_MUTEX_STATUS_SUCCESSFUL );
  (void) core_mutex_status;

  /*
   *  Switch ourself out because we blocked as a result of the
   *  _Thread_queue_Enqueue_critical().
   */

  _Thread_Dispatch_enable( cpu_self );

  status = (int) executing->Wait.return_code;

  /*
   *  If the thread is interrupted, while in the thread queue, by
   *  a POSIX signal, then pthread_cond_wait returns spuriously,
   *  according to the POSIX standard. It means that pthread_cond_wait
   *  returns a success status, except for the fact that it was not
   *  woken up a pthread_cond_signal() or a pthread_cond_broadcast().
   */

  if ( status == EINTR ) {
    status = 0;
  }

  /*
   *  When we get here the dispatch disable level is 0.
   */

  mutex_status = pthread_mutex_lock( mutex );
  if ( mutex_status != 0 ) {
    return EINVAL;
  }

  return status;
}
