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
 *  Copyright (c) 2016 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/spinlockimpl.h>

int pthread_spin_destroy( pthread_spinlock_t *spinlock )
{
#if defined(RTEMS_SMP)
  POSIX_Spinlock_Control *the_spinlock;

  the_spinlock = _POSIX_Spinlock_Get( spinlock );
  _SMP_ticket_lock_Destroy( &the_spinlock->Lock );
#endif
  return 0;
}
