/**
 *  @file
 *
 *  @brief Wait at a Spinlock
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

int pthread_spin_lock( pthread_spinlock_t *spinlock )
{
  POSIX_Spinlock_Control *the_spinlock;
  ISR_lock_Context        lock_context;
  CORE_spinlock_Status    status;

  the_spinlock = _POSIX_Spinlock_Get( spinlock, &lock_context );
  if ( the_spinlock == NULL ) {
    return EINVAL;
  }

  status = _CORE_spinlock_Seize(
    &the_spinlock->Spinlock,
    true,
    0,
    &lock_context
  );
  return _POSIX_Spinlock_Translate_core_spinlock_return_code( status );
}
