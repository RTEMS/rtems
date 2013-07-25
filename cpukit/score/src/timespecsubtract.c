/**
 *  @file  score/src/timespecsubtract.c
 *
 *  @brief Subtract Two Timespec
 *  @ingroup Timespec
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

#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>

void _Timespec_Subtract(
  const struct timespec *start,
  const struct timespec *end,
  struct timespec       *result
)
{

  if (end->tv_nsec < start->tv_nsec) {
    result->tv_sec  = end->tv_sec - start->tv_sec - 1;
    result->tv_nsec =
      (TOD_NANOSECONDS_PER_SECOND - start->tv_nsec) + end->tv_nsec;
  } else {
    result->tv_sec  = end->tv_sec - start->tv_sec;
    result->tv_nsec = end->tv_nsec - start->tv_nsec;
  }
}
