/**
 * @file rtems/posix/time.h
 */

/*
 *
 *
 *  $Id$
 */

#ifndef _RTEMS_POSIX_TIME_H
#define _RTEMS_POSIX_TIME_H

#include <rtems/score/tod.h>

/*
 *  Seconds from January 1, 1970 to January 1, 1988.  Used to account for
 *  differences between POSIX API and RTEMS core.
 *
 *  XXX probably able to be removed once core switches completely to timespec
 *  XXX for current TOD.
 */
#define POSIX_TIME_SECONDS_1970_THROUGH_1988 TOD_SECONDS_1970_THROUGH_1988

/*
 *  _POSIX_Timespec_subtract
 */

void _POSIX_Timespec_subtract(
  const struct timespec *the_start,
  const struct timespec *end,
  struct timespec *result
);

/*
 *  _POSIX_Timespec_to_interval
 */

Watchdog_Interval _POSIX_Timespec_to_interval(
  const struct timespec *time
);

/*
 *  _POSIX_Interval_to_timespec
 */

void _POSIX_Interval_to_timespec(
  Watchdog_Interval  ticks,
  struct timespec   *time
);

/*
 *  _POSIX_Absolute_timeout_to_ticks
 */
int _POSIX_Absolute_timeout_to_ticks(
  const struct timespec *abstime,
  Watchdog_Interval     *ticks_out
);

#endif
