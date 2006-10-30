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
  Watchdog_Interval              ticks = 0;
  struct timespec                current_time;
  struct timespec                difference;
  Core_semaphore_Blocking_option blocking = CORE_SEMAPHORE_BLOCK_WITH_TIMEOUT;

  /*
   *  Error check the absolute time to timeout
   */
#if 0
  if ( /* abstime->tv_sec < 0 || */ abstime->tv_nsec ) /* tv_sec is unsigned */
    blocking = CORE_SEMAPHORE_BAD_TIMEOUT_VALUE;
  else
#endif
  if ( abstime->tv_nsec >= TOD_NANOSECONDS_PER_SECOND )
    blocking = CORE_SEMAPHORE_BAD_TIMEOUT_VALUE;
  else { 
    (void) clock_gettime( CLOCK_REALTIME, &current_time );
    /*
     *  Make sure the abstime is in the future
     */
    if ( abstime->tv_sec < current_time.tv_sec )
      blocking = CORE_SEMAPHORE_BAD_TIMEOUT_VALUE;
    else if ( (abstime->tv_sec == current_time.tv_sec) &&
         (abstime->tv_nsec <= current_time.tv_nsec) )
      blocking = CORE_SEMAPHORE_BAD_TIMEOUT_VALUE;
    else {
      _POSIX_Timespec_subtract( &current_time, abstime, &difference );
      ticks = _POSIX_Timespec_to_interval( &difference );
      blocking = CORE_SEMAPHORE_BLOCK_WITH_TIMEOUT;
    }
   }

   return _POSIX_Semaphore_Wait_support( sem, blocking, ticks );
}
