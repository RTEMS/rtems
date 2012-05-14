/**
 *  @file  score/src/timespecdividebyinteger.c
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <sys/types.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>

void _Timespec_Divide_by_integer(
  const struct timespec *time,
  uint32_t               iterations,
  struct timespec       *result
)
{
  uint64_t t;

  /*
   *  For math simplicity just convert the timespec to nanoseconds
   *  in a 64-bit integer.
   */
  t  = time->tv_sec;
  t *= TOD_NANOSECONDS_PER_SECOND;
  t += time->tv_nsec;

  /*
   *  Divide to get nanoseconds per iteration
   */

  t /= iterations;

  /*
   *  Put it back in the timespec result
   */

  result->tv_sec  = t / TOD_NANOSECONDS_PER_SECOND;
  result->tv_nsec = t % TOD_NANOSECONDS_PER_SECOND;
}
