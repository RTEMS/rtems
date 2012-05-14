/*
 *  POSIX RWLock Manager -- Attempt to Obtain a Write Lock on a RWLock Instance
 *
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/posix/rwlock.h>
#include <rtems/posix/time.h>

/*
 *  pthread_rwlock_timedwrlock
 *
 *  This directive attempts to obtain a write only lock on an rwlock instance.
 *
 *  Input parameters:
 *    rwlock          - pointer to rwlock id
 *
 *  Output parameters:
 *    0          - if successful
 *    error code - if unsuccessful
 */

int pthread_rwlock_timedwrlock(
  pthread_rwlock_t      *rwlock,
  const struct timespec *abstime
)
{
  POSIX_RWLock_Control                        *the_rwlock;
  Objects_Locations                            location;
  Watchdog_Interval                            ticks;
  bool                                         do_wait = true;
  POSIX_Absolute_timeout_conversion_results_t  status;

  if ( !rwlock )
    return EINVAL;

  /*
   *  POSIX requires that blocking calls with timeouts that take
   *  an absolute timeout must ignore issues with the absolute
   *  time provided if the operation would otherwise succeed.
   *  So we check the abstime provided, and hold on to whether it
   *  is valid or not.  If it isn't correct and in the future,
   *  then we do a polling operation and convert the UNSATISFIED
   *  status into the appropriate error.
   *
   *  If the status is POSIX_ABSOLUTE_TIMEOUT_INVALID,
   *  POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST, or POSIX_ABSOLUTE_TIMEOUT_IS_NOW,
   *  then we should not wait.
   */
  status = _POSIX_Absolute_timeout_to_ticks( abstime, &ticks );
  if ( status != POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE )
    do_wait = false;

  the_rwlock = _POSIX_RWLock_Get( rwlock, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      _CORE_RWLock_Obtain_for_writing(
	&the_rwlock->RWLock,
	*rwlock,
	do_wait,
	ticks,
	NULL
      );

      _Thread_Enable_dispatch();
      if ( !do_wait &&
           (_Thread_Executing->Wait.return_code == CORE_RWLOCK_UNAVAILABLE) ) {
	if ( status == POSIX_ABSOLUTE_TIMEOUT_INVALID )
	  return EINVAL;
	if ( status == POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST ||
	     status == POSIX_ABSOLUTE_TIMEOUT_IS_NOW )
	  return ETIMEDOUT;
      }

      return _POSIX_RWLock_Translate_core_RWLock_return_code(
        (CORE_RWLock_Status) _Thread_Executing->Wait.return_code
      );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
