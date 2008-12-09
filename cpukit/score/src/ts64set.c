/**
 * @file score/src/ts64set.c
*/

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/timestamp.h>

#if defined(CPU_RTEMS_SCORE_TIMESTAMP_IS_INT64) && \
    !defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
void _Timestamp64_Set(
  Timestamp64_Control *_time,
  long                 _seconds,
  long                 _nanoseconds
)
{
  int64_t time;

  time = (int64_t)_seconds * 1000000000;
  time += (int64_t)_nanoseconds;
  *_time = time;
}
#endif
