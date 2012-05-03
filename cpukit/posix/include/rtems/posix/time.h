/**
 * @file rtems/posix/time.h
 *
 * This defines the interface to implementation helper routines related
 * to POSIX time types.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_TIME_H
#define _RTEMS_POSIX_TIME_H

#include <rtems/score/timespec.h>
#include <rtems/score/watchdog.h>

/**
 *  @brief Absolute Timeout Conversion Results
 *
 *  This enumeration defines the possible results of converting
 *  an absolute time used for timeouts to POSIX blocking calls to
 *  a number of ticks.
 */
typedef enum {
  /** The timeout is invalid. */
  POSIX_ABSOLUTE_TIMEOUT_INVALID,
  /** The timeout represents a time that is in the past. */
  POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST,
  /** The timeout represents a time that is equal to the current time. */
  POSIX_ABSOLUTE_TIMEOUT_IS_NOW,
  /** The timeout represents a time that is in the future. */
  POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE,
} POSIX_Absolute_timeout_conversion_results_t;

/**
 *  @brief Convert Absolute Timeout to Ticks
 *
 *  This method takes an absolute time being used as a timeout
 *  to a blocking directive, validates it and returns the number
 *  of corresponding clock ticks for use by the SuperCore.
 *
 *  @param[in] abstime is the timeout
 *  @param[in] ticks_out will contain the number of ticks
 *
 *  @return This method returns the number of ticks in @a ticks_out
 *          and a status value indicating whether the absolute time
 *          is valid, in the past, equal to the current time or in
 *          the future as it should be.
 */
POSIX_Absolute_timeout_conversion_results_t _POSIX_Absolute_timeout_to_ticks(
  const struct timespec *abstime,
  Watchdog_Interval     *ticks_out
);

#endif
