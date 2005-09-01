/*
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
  /*
   *  The abstime is a walltime.  We turn it into an interval.
   */
  Watchdog_Interval ticks;
  struct timespec   current_time;
  struct timespec   difference;

  /*
   *  Error check the absolute time to timeout
   */
  if ( /* abstime->tv_sec < 0 || */ abstime->tv_nsec ) /* tv_sec is unsigned */
    return EINVAL;

  if ( abstime->tv_nsec >= TOD_NANOSECONDS_PER_SECOND )
    return EINVAL;
  
  (void) clock_gettime( CLOCK_REALTIME, &current_time );

  /*
   *  Make sure the abstime is in the future
   */
  if ( abstime->tv_sec < current_time.tv_sec )
    return EINVAL;
  if ( (abstime->tv_sec == current_time.tv_sec) &&
       (abstime->tv_nsec <= current_time.tv_nsec) )
    return EINVAL;

  _POSIX_Timespec_subtract( &current_time, abstime, &difference );

  ticks = _POSIX_Timespec_to_interval( &difference );

  return _POSIX_Semaphore_Wait_support( sem, TRUE, ticks );
}

