/**
 * @file score/src/ts64dividebyinteger.c
*/

/*
 *  COPYRIGHT (c) 1989-2008.
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

#if CPU_TIMESTAMP_USE_INT64 == TRUE
void _Timestamp64_Divide_by_integer(
  const Timestamp64_Control *_time,
  uint32_t                   _iterations,
  Timestamp64_Control       *_result
)
{
  _Timestamp64_implementation_Divide_by_integer( _time, _iterations, _result );
}
#endif
