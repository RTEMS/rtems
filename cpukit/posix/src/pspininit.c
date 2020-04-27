/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief POSIX Function Initializes a Spinlock Instance
 */

/*
 *  POSIX Spinlock Manager -- Initialize a Spinlock Instance
 *
 *  COPYRIGHT (c) 1989-2006.
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

int pthread_spin_init(
  pthread_spinlock_t  *spinlock,
  int                  pshared
)
{
#if defined(RTEMS_SMP)
  POSIX_Spinlock_Control *the_spinlock;

  the_spinlock = _POSIX_Spinlock_Get( spinlock );
  _SMP_ticket_lock_Initialize( &the_spinlock->Lock );
#endif
  return 0;
}
