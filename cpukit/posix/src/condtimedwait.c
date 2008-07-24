/*
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
  Watchdog_Interval ticks;
  boolean           already_timedout;

  /*
   *  POSIX requires that blocking calls with timeouts that take
   *  an absolute timeout must ignore issues with the absolute
   *  time provided if the operation would otherwise succeed.
   *  So we check the abstime provided, and hold on to whether it
   *  is valid or not.  If it isn't correct and in the future,
   *  then we do a polling operation and convert the UNSATISFIED
   *  status into the appropriate error.
   */
  switch ( _POSIX_Absolute_timeout_to_ticks(abstime, &ticks) ) {
    case POSIX_ABSOLUTE_TIMEOUT_INVALID:
      return EINVAL;
    case POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST:
    case POSIX_ABSOLUTE_TIMEOUT_IS_NOW:
      already_timedout = TRUE;
      break;
    case POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE:
      already_timedout = FALSE;
      break;
  }

  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    ticks,
    already_timedout
  );
}
