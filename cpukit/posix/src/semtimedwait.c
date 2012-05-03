/*
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

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

/*
 *  11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 *  NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */

int sem_timedwait(
  sem_t                 *sem,
  const struct timespec *abstime
)
{
  Watchdog_Interval                            ticks;
  bool                                         do_wait = true;
  POSIX_Absolute_timeout_conversion_results_t  status;
  int                                          lock_status;

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

  lock_status = _POSIX_Semaphore_Wait_support( sem, do_wait, ticks );

  /*
   *  This service only gives us the option to block.  We used a polling
   *  attempt to obtain if the abstime was not in the future.  If we did
   *  not obtain the semaphore, then not look at the status immediately,
   *  make sure the right reason is returned.
   */
  if ( !do_wait && (lock_status == EBUSY) ) {
    if ( lock_status == POSIX_ABSOLUTE_TIMEOUT_INVALID )
      rtems_set_errno_and_return_minus_one( EINVAL );
    if ( lock_status == POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST ||
         lock_status == POSIX_ABSOLUTE_TIMEOUT_IS_NOW )
      rtems_set_errno_and_return_minus_one( ETIMEDOUT );
  }

  return lock_status;
}
