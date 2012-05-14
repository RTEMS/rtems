/*
 *  POSIX Spinlock Manager -- Destroy a Spinlock
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
 *  pthread_spin_destroy
 *
 *  This directive allows a thread to delete a spinlock specified by
 *  the spinlock id.  The spinlock is freed back to the inactive
 *  spinlock chain.
 *
 *  Input parameters:
 *    spinlock - spinlock id
 *
 *  Output parameters:
 *    0           - if successful
 *    error code  - if unsuccessful
 */

int pthread_spin_destroy(
  pthread_spinlock_t *spinlock
)
{
  POSIX_Spinlock_Control *the_spinlock = NULL;
  Objects_Locations      location;

  if ( !spinlock )
    return EINVAL;

  the_spinlock = _POSIX_Spinlock_Get( spinlock, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( _CORE_spinlock_Is_busy( &the_spinlock->Spinlock ) ) {
        _Thread_Enable_dispatch();
        return EBUSY;
      }

      _Objects_Close( &_POSIX_Spinlock_Information, &the_spinlock->Object );

      _POSIX_Spinlock_Free( the_spinlock );

      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
