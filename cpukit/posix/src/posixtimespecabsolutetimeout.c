/*
 *  Convert abstime timeout to ticks
 */

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
  _TOD_Get( &current_time );

  if ( _Timespec_Less_than( abstime, &current_time ) )
    return POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST;

  /*
   *  How long until the requested absolute time?
   */
  _Timespec_Subtract( &current_time, abstime, &difference );

  /*
   *  Internally the SuperCore uses ticks, so convert to them.
   */
  *ticks_out = _Timespec_To_ticks( &difference );

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

