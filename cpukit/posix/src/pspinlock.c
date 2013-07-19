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
 *  This directive allows a thread to wait at a spinlock.
 *
 *  @param[in] spinlock is spinlock id
 * 
 *  @return This method returns 0 if there was not an
 *  error. Otherwise, a status code is returned indicating the
 *  source of the error.
 */
int pthread_spin_lock(
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
      status = _CORE_spinlock_Wait( &the_spinlock->Spinlock, true, 0 );
      _Objects_Put( &the_spinlock->Object );
      return _POSIX_Spinlock_Translate_core_spinlock_return_code( status );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
