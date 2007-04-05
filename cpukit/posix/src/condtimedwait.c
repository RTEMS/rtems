/*
 *  $Id$
 */

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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/time.h>
#include <rtems/posix/mutex.h>

/*PAGE
 *
 *  11.4.4 Waiting on a Condition, P1003.1c/Draft 10, p. 105
 */

int pthread_cond_timedwait(
  pthread_cond_t        *cond,
  pthread_mutex_t       *mutex,
  const struct timespec *abstime
)
{
  Watchdog_Interval timeout;
  struct timespec   current_time;
  struct timespec   difference;
  boolean           already_timedout = FALSE;

  if ( !_Timespec_Is_valid(abstime) )
    return EINVAL;

  /*
   *  The abstime is a walltime.  We turn it into an interval.
   */

  _TOD_Get( &current_time );

  /*
   * Make sure the timeout is in the future
   */
  if ( _Timespec_Less_than( abstime, &current_time ) ) {
    already_timedout = TRUE;
    difference.tv_sec  = 0;
    difference.tv_nsec = 0;
    timeout = 0;
  } else {
    _Timespec_Subtract( &current_time, abstime, &difference );
    timeout = _Timespec_To_ticks( &difference );
  }

  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    timeout,
    already_timedout
  );
}
