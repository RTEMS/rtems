/*
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

/*PAGE
 *
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
  boolean                                      do_wait = TRUE;
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
   */
  status = _POSIX_Absolute_timeout_to_ticks( abstime, &ticks );
  switch ( status ) {
    case POSIX_ABSOLUTE_TIMEOUT_INVALID:
    case POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST:
    case POSIX_ABSOLUTE_TIMEOUT_IS_NOW:
      do_wait = FALSE;
      break;
    case POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE:
      do_wait = TRUE;
      break;
  }

  lock_status = _POSIX_Semaphore_Wait_support( sem, do_wait, ticks );

  /*
   *  This service only gives us the option to block.  We used a polling
   *  attempt to obtain if the abstime was not in the future.  If we did
   *  not obtain the semaphore, then not look at the status immediately,
   *  make sure the right reason is returned.
   */
  if ( !do_wait && (lock_status == EBUSY) ) {
    switch (lock_status) {
      case POSIX_ABSOLUTE_TIMEOUT_INVALID:
        return EINVAL;
      case POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST:
      case POSIX_ABSOLUTE_TIMEOUT_IS_NOW:
        return ETIMEDOUT;
      case POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE:
        break;
    }
  }

  return lock_status;
}
