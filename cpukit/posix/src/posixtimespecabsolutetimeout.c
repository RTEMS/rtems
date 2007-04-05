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

/*
 *  The abstime is a walltime.  We turn it into an interval.
 */
int _POSIX_Absolute_timeout_to_ticks(
  const struct timespec *abstime,
  Watchdog_Interval     *ticks_out
)
{
  struct timespec current_time;
  struct timespec difference;

  if ( !_Timespec_Is_valid(abstime) )
    return EINVAL;

  _TOD_Get( &current_time );

  /*
   *  Make sure the abstime is in the future
   */
  if ( _Timespec_Less_than( abstime, &current_time ) )
    return EINVAL;

  _Timespec_Subtract( &current_time, abstime, &difference );

  *ticks_out = _Timespec_To_ticks( &difference );

  return 0;
}

