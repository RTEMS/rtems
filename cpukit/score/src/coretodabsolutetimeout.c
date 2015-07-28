/**
 * @file
 *
 * @brief Convert Absolute Timeout to Ticks
 * @ingroup ScoreTOD
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>

/*
 *  The abstime is a walltime.  We turn it into an interval.
 */
TOD_Absolute_timeout_conversion_results _TOD_Absolute_timeout_to_ticks(
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
    return TOD_ABSOLUTE_TIMEOUT_INVALID;

  /*
   *  Is the absolute time in the past?
   */
  _TOD_Get_as_timespec( &current_time );

  if ( _Timespec_Less_than( abstime, &current_time ) )
    return TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST;

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
    return TOD_ABSOLUTE_TIMEOUT_IS_NOW;

  /*
   *  This is the case we were expecting and it took this long to
   *  get here.
   */
  return TOD_ABSOLUTE_TIMEOUT_IS_IN_FUTURE;
}

