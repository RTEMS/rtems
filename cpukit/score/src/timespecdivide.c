/**
 *  @file  score/src/timespecdivide.c
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

void _Timespec_Divide(
  const struct timespec *lhs,
  const struct timespec *rhs,
  uint32_t              *ival_percentage,
  uint32_t              *fval_percentage
)
{
  uint64_t left, right, answer;

  /*
   *  For math simplicity just convert the timespec to nanoseconds
   *  in a 64-bit integer.
   */
  left   = lhs->tv_sec * (uint64_t)TOD_NANOSECONDS_PER_SECOND;
  left  += lhs->tv_nsec;
  right  = rhs->tv_sec * (uint64_t)TOD_NANOSECONDS_PER_SECOND;
  right += rhs->tv_nsec;

  if ( right == 0 ) {
    *ival_percentage = 0;
    *fval_percentage = 0;
    return;
  }

  /*
   *  Put it back in the timespec result.
   *
   *  TODO: Rounding on the last digit of the fval.
   */

  answer = (left * 100000) / right;

  *ival_percentage = answer / 1000;
  *fval_percentage = answer % 1000;
}
