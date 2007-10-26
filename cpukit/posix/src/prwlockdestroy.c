/*
 *  POSIX RWLock Manager -- Destroy a RWLock
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/rwlock.h>

/*
 *  pthread_rwlock_destroy
 *
 *  This directive allows a thread to delete a rwlock specified by
 *  the rwlock id.  The rwlock is freed back to the inactive
 *  rwlock chain.
 *
 *  Input parameters:
 *    rwlock - rwlock id
 *
 *  Output parameters:
 *    0           - if successful
 *    error code  - if unsuccessful
 */

int pthread_rwlock_destroy(
  pthread_rwlock_t *rwlock
)
{
  POSIX_RWLock_Control *the_rwlock = NULL;
  Objects_Locations      location;

  if ( !rwlock )
    return EINVAL;

  the_rwlock = _POSIX_RWLock_Get( rwlock, &location );
  switch ( location ) {

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      return EINVAL;

    case OBJECTS_LOCAL:
#if 0
      if ( the_rwlock->RWLock.number_of_waiting_threads != 0 ) {
        _Thread_Enable_dispatch();
        return EBUSY;
      }
#endif

      _Objects_Close( &_POSIX_RWLock_Information, &the_rwlock->Object );

      _POSIX_RWLock_Free( the_rwlock );

      _Thread_Enable_dispatch();
      return 0;
  }

  return POSIX_BOTTOM_REACHED();
}
