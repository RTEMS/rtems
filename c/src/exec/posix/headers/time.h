/*
 *
 *
 *  $Id$
 */

#ifndef __RTEMS_POSIX_TIME_h
#define __RTEMS_POSIX_TIME_h

#include <rtems/score/tod.h>

/*
 *  Seconds from January 1, 1970 to January 1, 1988.  Used to account for
 *  differences between POSIX API and RTEMS core.
 */
 
#define POSIX_TIME_SECONDS_1970_THROUGH_1988 \
  (((1987 - 1970 + 1)  * TOD_SECONDS_PER_NON_LEAP_YEAR) + \
  (4 * TOD_SECONDS_PER_DAY))

/*PAGE
 *
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

/*PAGE
 *
 *  _POSIX_Interval_to_timespec
 */
 
void _POSIX_Interval_to_timespec(
  Watchdog_Interval  ticks,
  struct timespec   *time
);

#endif
