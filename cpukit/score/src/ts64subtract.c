/**
 * @file score/src/ts64subtract.c
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
void _Timestamp64_Subtract(
  const Timestamp64_Control *_start,
  const Timestamp64_Control *_end,
  Timestamp64_Control       *_result
)
{
  _Timestamp64_implementation_Subtract( _start, _end, _result );
}
#endif
