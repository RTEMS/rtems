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
  POSIX_Mutex_Control *the_mutex;
  ISR_lock_Context     lock_context;
  int                  eno;

  _Objects_Allocator_lock();

  the_mutex = _POSIX_Mutex_Get_interrupt_disable( mutex, &lock_context );

  if ( the_mutex != NULL ) {
    _CORE_mutex_Acquire_critical( &the_mutex->Mutex, &lock_context );

    /*
     * XXX: There is an error for the mutex being locked
     *  or being in use by a condition variable.
     */

    if ( !_CORE_mutex_Is_locked( &the_mutex->Mutex ) ) {
      _Objects_Close( &_POSIX_Mutex_Information, &the_mutex->Object );
      _CORE_mutex_Release( &the_mutex->Mutex, &lock_context );
      _CORE_mutex_Destroy( &the_mutex->Mutex );
      _POSIX_Mutex_Free( the_mutex );
      eno = 0;
    } else {
      _CORE_mutex_Release( &the_mutex->Mutex, &lock_context );
      eno = EBUSY;
    }
  } else {
    eno = EINVAL;
  }

  _Objects_Allocator_unlock();
  return eno;
}
