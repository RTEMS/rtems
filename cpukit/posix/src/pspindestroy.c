/**
 *  @file
 *
 *  @brief Destroy a Spinlock
 *  @ingroup POSIXAPI
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

#include <rtems/posix/spinlockimpl.h>

#include <errno.h>

int pthread_spin_destroy( pthread_spinlock_t *spinlock )
{
  POSIX_Spinlock_Control *the_spinlock;
  ISR_lock_Context        lock_context;

  _Objects_Allocator_lock();

  the_spinlock = _POSIX_Spinlock_Get( spinlock, &lock_context );
  if ( the_spinlock == NULL ) {
    _Objects_Allocator_unlock();
    return EINVAL;
  }

  _CORE_spinlock_Acquire_critical( &the_spinlock->Spinlock, &lock_context );

  if ( _CORE_spinlock_Is_busy( &the_spinlock->Spinlock ) ) {
    _CORE_spinlock_Release( &the_spinlock->Spinlock, &lock_context );
    _Objects_Allocator_unlock();
    return EBUSY;
  }

  _CORE_spinlock_Release( &the_spinlock->Spinlock, &lock_context );

  _Objects_Close( &_POSIX_Spinlock_Information, &the_spinlock->Object );
  _POSIX_Spinlock_Free( the_spinlock );
  _Objects_Allocator_unlock();
  return 0;
}
