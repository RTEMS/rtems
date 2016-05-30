/**
 * @file
 *
 * @brief Support Call to function Enables Locking of Mutex Object 
 * @ingroup POSIXAPI
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

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/posixapi.h>

THREAD_QUEUE_OBJECT_ASSERT(
  POSIX_Mutex_Control,
  Mutex.Recursive.Mutex.Wait_queue
);

static Status_Control _POSIX_Mutex_Lock_nested(
  CORE_recursive_mutex_Control *the_recursive_mutex
)
{
  POSIX_Mutex_Control *the_mutex;

  the_mutex = RTEMS_CONTAINER_OF(
    the_recursive_mutex,
    POSIX_Mutex_Control,
    Mutex.Recursive
  );

  if ( the_mutex->is_recursive ) {
    return _CORE_recursive_mutex_Seize_nested( the_recursive_mutex );
  } else {
    return STATUS_NESTING_NOT_ALLOWED;
  }
}

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t   *mutex,
  bool               wait,
  Watchdog_Interval  timeout
)
{
  POSIX_Mutex_Control  *the_mutex;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  Status_Control        status;

  the_mutex = _POSIX_Mutex_Get( mutex, &queue_context );

  if ( the_mutex == NULL ) {
    return EINVAL;
  }

  executing = _Thread_Executing;

  switch ( the_mutex->protocol ) {
    case POSIX_MUTEX_PRIORITY_CEILING:
      status = _CORE_ceiling_mutex_Seize(
        &the_mutex->Mutex,
        executing,
        wait,
        timeout,
        _POSIX_Mutex_Lock_nested,
        &queue_context
      );
      break;
    case POSIX_MUTEX_NO_PROTOCOL:
      status = _CORE_recursive_mutex_Seize_no_protocol(
        &the_mutex->Mutex.Recursive,
        POSIX_MUTEX_NO_PROTOCOL_TQ_OPERATIONS,
        executing,
        wait,
        timeout,
        _POSIX_Mutex_Lock_nested,
        &queue_context
      );
      break;
    default:
      _Assert( the_mutex->protocol == POSIX_MUTEX_PRIORITY_INHERIT );
      status = _CORE_recursive_mutex_Seize(
        &the_mutex->Mutex.Recursive,
        executing,
        wait,
        timeout,
        _POSIX_Mutex_Lock_nested,
        &queue_context
      );
      break;
  }

  return _POSIX_Get_error( status );
}
