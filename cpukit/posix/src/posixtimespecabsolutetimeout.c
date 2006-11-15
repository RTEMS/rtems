/*
 *  Convert abstime timeout to ticks
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

/*
 *  The abstime is a walltime.  We turn it into an interval.
 */
int _POSIX_Absolute_timeout_to_ticks(
  const struct timespec *abstime,
  Watchdog_Interval     *ticks_out
)
{
  struct timespec   current_time;
  struct timespec   difference;

  if ( !abstime )
    return EINVAL;

  /*
   *  Error check the absolute time to timeout
   */
#if 0
   /* they are unsigned so this is impossible */
  if ( abstime->tv_sec < 0 || abstime->tv_nsec < 0 )
    return EINVAL;
#endif

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

  *ticks_out = _POSIX_Timespec_to_interval( &difference );

  return 0;
}

