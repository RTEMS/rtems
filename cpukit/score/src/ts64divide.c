/**
 *  @file  score/src/ts64divide.c
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

#include <rtems/score/timestamp.h>

/* This method is never inlined. */
#if CPU_TIMESTAMP_USE_INT64 == TRUE || CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
void _Timestamp64_Divide(
  const Timestamp64_Control *_lhs,
  const Timestamp64_Control *_rhs,
  uint32_t                  *_ival_percentage,
  uint32_t                  *_fval_percentage
)
{
  Timestamp64_Control answer;

  if ( *_rhs == 0 ) {
    *_ival_percentage = 0;
    *_fval_percentage = 0;
    return;
  }

  /*
   *  This looks odd but gives the results the proper precision.
   *
   *  TODO: Rounding on the last digit of the fval.
   */

  answer = (*_lhs * 100000) / *_rhs;

  *_ival_percentage = answer / 1000;
  *_fval_percentage = answer % 1000;
}
#endif
