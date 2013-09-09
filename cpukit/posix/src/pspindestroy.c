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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/spinlockimpl.h>

/**
 *  This directive allows a thread to delete a spinlock specified by
 *  the spinlock id.  The spinlock is freed back to the inactive
 *  spinlock chain.
 *
 *  @param[in] spinlock is the spinlock id
 *
 *  @return This method returns 0 if there was not an
 *  error. Otherwise, a status code is returned indicating the
 *  source of the error.
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
        _Objects_Put( &the_spinlock->Object );
        return EBUSY;
      }

      _Objects_Close( &_POSIX_Spinlock_Information, &the_spinlock->Object );

      _POSIX_Spinlock_Free( the_spinlock );

      _Objects_Put( &the_spinlock->Object );
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
