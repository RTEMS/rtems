/**
 * @file
 *
 * @brief Initializing and Destroying a Mutex
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

/*
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 */

int pthread_mutex_destroy(
  pthread_mutex_t           *mutex
)
{
  POSIX_Mutex_Control  *the_mutex;
  Thread_queue_Context  queue_context;
  int                   eno;

  _Objects_Allocator_lock();

  the_mutex = _POSIX_Mutex_Get( mutex, &queue_context );

  if ( the_mutex != NULL ) {
    _POSIX_Mutex_Acquire_critical( the_mutex, &queue_context );

    /*
     * XXX: There is an error for the mutex being locked
     *  or being in use by a condition variable.
     */

    if (
      !_CORE_mutex_Is_locked( &the_mutex->Mutex.Recursive.Mutex )
    ) {
      _Objects_Close( &_POSIX_Mutex_Information, &the_mutex->Object );
      _POSIX_Mutex_Release( the_mutex, &queue_context );
      _CORE_mutex_Destroy( &the_mutex->Mutex.Recursive.Mutex );
      _POSIX_Mutex_Free( the_mutex );
      eno = 0;
    } else {
      _POSIX_Mutex_Release( the_mutex, &queue_context );
      eno = EBUSY;
    }
  } else {
    eno = EINVAL;
  }

  _Objects_Allocator_unlock();
  return eno;
}
