/*
 *  POSIX Spinlock Manager -- Wait at a Spinlock
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/spinlock.h>

/*
 *  pthread_spin_unlock
 *
 *  This directive allows a thread to wait at a spinlock.
 *
 *  Input parameters:
 *    spinlock    - spinlock id
 *
 *  Output parameters:
 *    0          - if successful
 *    error code - if unsuccessful
 */

int pthread_spin_unlock(
  pthread_spinlock_t *spinlock
)
{
  POSIX_Spinlock_Control  *the_spinlock = NULL;
  Objects_Locations        location;
  CORE_spinlock_Status     status;

  if ( !spinlock )
    return EINVAL;

  the_spinlock = _POSIX_Spinlock_Get( spinlock, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      status = _CORE_spinlock_Release( &the_spinlock->Spinlock );
      _Thread_Enable_dispatch();
      return _POSIX_Spinlock_Translate_core_spinlock_return_code( status );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
