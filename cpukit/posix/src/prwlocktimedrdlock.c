/*
 *  POSIX RWLock Manager -- Attempt to Obtain a Read Lock on a RWLock Instance
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
#include <rtems/posix/time.h>

/*
 *  pthread_rwlock_timedrdlock
 *
 *  This directive attempts to obtain a read only lock on an rwlock instance.
 *
 *  Input parameters:
 *    rwlock          - pointer to rwlock id
 *
 *  Output parameters:
 *    0          - if successful
 *    error code - if unsuccessful
 */

int pthread_rwlock_timedrdlock(
  pthread_rwlock_t      *rwlock,
  const struct timespec *abstime
)
{
  POSIX_RWLock_Control  *the_rwlock;
  Objects_Locations      location;
  Watchdog_Interval      ticks;
  int                    status;
  
  if ( !rwlock )
    return EINVAL;

  status = _POSIX_Absolute_timeout_to_ticks( abstime, &ticks );
  if ( !status )
    return status;

  the_rwlock = _POSIX_RWLock_Get( rwlock, &location );
  switch ( location ) {

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      return EINVAL;

    case OBJECTS_LOCAL:

      _CORE_RWLock_Obtain_for_reading(
	&the_rwlock->RWLock,
	*rwlock,
	TRUE,                   // we are willing to wait up to ticks
	ticks,
	NULL
      );

      _Thread_Enable_dispatch();
      return _POSIX_RWLock_Translate_core_RWLock_return_code(
        (CORE_RWLock_Status) _Thread_Executing->Wait.return_code
      );
  }

  return POSIX_BOTTOM_REACHED();
}
