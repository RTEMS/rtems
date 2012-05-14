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

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/time.h>
#include <rtems/posix/mutex.h>

/*
 *  11.4.4 Waiting on a Condition, P1003.1c/Draft 10, p. 105
 */

int pthread_cond_timedwait(
  pthread_cond_t        *cond,
  pthread_mutex_t       *mutex,
  const struct timespec *abstime
)
{
  Watchdog_Interval                            ticks;
  bool                                         already_timedout;
  POSIX_Absolute_timeout_conversion_results_t  status;

  /*
   *  POSIX requires that blocking calls with timeouts that take
   *  an absolute timeout must ignore issues with the absolute
   *  time provided if the operation would otherwise succeed.
   *  So we check the abstime provided, and hold on to whether it
   *  is valid or not.  If it isn't correct and in the future,
   *  then we do a polling operation and convert the UNSATISFIED
   *  status into the appropriate error.
   */
  already_timedout = false;
  status = _POSIX_Absolute_timeout_to_ticks(abstime, &ticks);
  if ( status == POSIX_ABSOLUTE_TIMEOUT_INVALID )
    return EINVAL;

  if ( status == POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST ||
       status == POSIX_ABSOLUTE_TIMEOUT_IS_NOW )
    already_timedout = true;

  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    ticks,
    already_timedout
  );
}
