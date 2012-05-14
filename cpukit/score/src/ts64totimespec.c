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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/timestamp.h>

#if CPU_TIMESTAMP_USE_INT64 == TRUE
void _Timestamp64_To_timespec(
  const Timestamp64_Control *_timestamp,
  struct timespec           *_timespec
)
{
  _Timestamp64_implementation_To_timespec( _timestamp, _timespec );
}
#endif
