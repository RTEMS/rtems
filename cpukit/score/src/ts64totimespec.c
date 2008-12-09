/**
 * @file score/src/ts64totimespec.c
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
void _Timestamp64_To_timespec(
  Timestamp64_Control *_timestamp,
  struct timespec     *_timespec
)
{
  _timespec->tv_sec = *_timestamp / 1000000000;
  _timespec->tv_nsec = *_timestamp % 1000000000;
}
#endif
