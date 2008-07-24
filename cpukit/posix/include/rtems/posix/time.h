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

/*  Absolute Timeout Conversion Results
 *
 *  This enumeration defines the possible results of converting
 *  an absolute time used for timeouts to POSIX blocking calls to
 *  a number of ticks.
 */
typedef enum {
  /* The timeout is invalid. */
  POSIX_ABSOLUTE_TIMEOUT_INVALID,
  /* The timeout represents a time that is in the past. */
  POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST,
  /* The timeout represents a time that is equal to the current time. */
  POSIX_ABSOLUTE_TIMEOUT_IS_NOW,
  /* The timeout represents a time that is in the future. */
  POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE,
} POSIX_Absolute_timeout_conversion_results_t;

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

/*
 *  Convert Absolute Timeout to Ticks
 *
 *  This method takes an absolute time being used as a timeout
 *  to a blocking directive, validates it and returns the number
 *  of corresponding clock ticks for use by the SuperCore.
 *
 *  abstime   - is the timeout
 *  ticks_out -  will contain the number of ticks
 *
 *  This method returns the number of ticks in @a ticks_out
 *  and a status value indicating whether the absolute time
 *  is valid, in the past, equal to the current time or in 
 *  the future as it should be.
 */
POSIX_Absolute_timeout_conversion_results_t _POSIX_Absolute_timeout_to_ticks(
  const struct timespec *abstime,
  Watchdog_Interval     *ticks_out
);

#endif
