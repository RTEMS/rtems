/**
 *  @file
 *
 *  @brief Destroy a RWLock
 *  @ingroup POSIXAPI
 */
/*
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
#include <rtems/posix/rwlockimpl.h>

/**
 *  This directive allows a thread to delete a rwlock specified by
 *  the rwlock id.  The rwlock is freed back to the inactive
 *  rwlock chain.
 *
 *  @param[in] rwlock is the rwlock id
 *
 *  @return This method returns 0 if there was not an
 *  error. Otherwise, a status code is returned indicating the
 *  source of the error.
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

    case OBJECTS_LOCAL:
      /*
       *  If there is at least one thread waiting, then do not delete it.
       */
      if ( _Thread_queue_First( &the_rwlock->RWLock.Wait_queue ) != NULL ) {
        _Objects_Put( &the_rwlock->Object );
        return EBUSY;
      }

      /*
       *  POSIX doesn't require behavior when it is locked.
       */

      _Objects_Close( &_POSIX_RWLock_Information, &the_rwlock->Object );

      _POSIX_RWLock_Free( the_rwlock );

      _Objects_Put( &the_rwlock->Object );
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
