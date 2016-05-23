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

THREAD_QUEUE_OBJECT_ASSERT( POSIX_Mutex_Control, Mutex.Wait_queue );

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t   *mutex,
  bool               blocking,
  Watchdog_Interval  timeout
)
{
  POSIX_Mutex_Control  *the_mutex;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;

  the_mutex = _POSIX_Mutex_Get( mutex, &queue_context );

  if ( the_mutex == NULL ) {
    return EINVAL;
  }

  executing = _Thread_Executing;
  _CORE_mutex_Seize(
    &the_mutex->Mutex,
    executing,
    blocking,
    timeout,
    &queue_context
  );
  return _POSIX_Mutex_Translate_core_mutex_return_code(
    (CORE_mutex_Status) executing->Wait.return_code
  );
}
