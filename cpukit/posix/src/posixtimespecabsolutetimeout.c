/*
 *  Convert abstime timeout to ticks
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

static boolean _Timespec_Is_valid(
  const struct timespec *time
)
{
  if ( !time )
    return FALSE;

  if ( time->tv_sec < 0 )
    return FALSE;

  if ( time->tv_nsec < 0 )
    return FALSE;

  if ( time->tv_nsec >= TOD_NANOSECONDS_PER_SECOND )
    return FALSE;

  return TRUE;
}

static boolean _Timespec_Less_than(
  const struct timespec *lhs,
  const struct timespec *rhs
)
{
  if ( lhs->tv_sec < rhs->tv_sec )
    return TRUE;

  if ( lhs->tv_sec > rhs->tv_sec )
    return FALSE;

  /* ASSERT: lhs->tv_sec == rhs->tv_sec */
  if ( lhs->tv_nsec < rhs->tv_nsec )
    return TRUE;

  return FALSE;
}


/*
 *  The abstime is a walltime.  We turn it into an interval.
 */
POSIX_Absolute_timeout_conversion_results_t _POSIX_Absolute_timeout_to_ticks(
  const struct timespec *abstime,
  Watchdog_Interval     *ticks_out
)
{
  struct timespec current_time;
  struct timespec difference;

  
  /*
   *  Make sure there is always a value returned.
   */
  *ticks_out = 0;

  /*
   *  Is the absolute time even valid?
   */
  if ( !_Timespec_Is_valid(abstime) )
    return POSIX_ABSOLUTE_TIMEOUT_INVALID;

  /*
   *  Is the absolute time in the past?
   */
  clock_gettime( CLOCK_REALTIME, &current_time );

  if ( _Timespec_Less_than( abstime, &current_time ) )
    return POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST;

  /*
   *  How long until the requested absolute time?
   */
  _POSIX_Timespec_subtract( &current_time, abstime, &difference );

  /*
   *  Internally the SuperCore uses ticks, so convert to them.
   */
  *ticks_out = _POSIX_Timespec_to_interval( &difference );

  /*
   *  If the difference was 0, then the future is now.  It is so bright
   *  we better wear shades.
   */
  if ( !*ticks_out )
    return POSIX_ABSOLUTE_TIMEOUT_IS_NOW;

  /*
   *  This is the case we were expecting and it took this long to
   *  get here.
   */
  return POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE;
}

