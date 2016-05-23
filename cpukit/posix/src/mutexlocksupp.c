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

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Mutex_Control, Mutex.Wait_queue );

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t   *mutex,
  bool               blocking,
  Watchdog_Interval  timeout
)
{
  POSIX_Mutex_Control  *the_mutex;
  Thread_queue_Context  queue_context;
  Status_Control        status;

  the_mutex = _POSIX_Mutex_Get( mutex, &queue_context );

  if ( the_mutex == NULL ) {
    return EINVAL;
  }

  status = _CORE_mutex_Seize(
    &the_mutex->Mutex,
    _Thread_Executing,
    blocking,
    timeout,
    &queue_context
  );
  return _POSIX_Get_error( status );
}
