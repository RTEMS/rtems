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
  Watchdog_Interval timeout;
  struct timespec   current_time;
  struct timespec   difference;
  boolean           already_timedout = FALSE;

  if ( !abstime )
    return EINVAL;

  /*
   *  The abstime is a walltime.  We turn it into an interval.
   */

  (void) clock_gettime( CLOCK_REALTIME, &current_time );

  /* XXX probably some error checking should go here */

  _POSIX_Timespec_subtract( &current_time, abstime, &difference );

  if ( ( difference.tv_sec < 0 ) || ( ( difference.tv_sec == 0 ) &&
       ( difference.tv_nsec < 0 ) ) )
    already_timedout = TRUE;   

  timeout = _POSIX_Timespec_to_interval( &difference );

  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    timeout,
    already_timedout
  );
}
