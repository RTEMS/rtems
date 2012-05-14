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

#include <time.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>

void _Timespec_From_ticks(
  uint32_t         ticks,
  struct timespec *time
)
{
  uint32_t    usecs;

  usecs = ticks * rtems_configuration_get_microseconds_per_tick();

  time->tv_sec  = usecs / TOD_MICROSECONDS_PER_SECOND;
  time->tv_nsec = (usecs % TOD_MICROSECONDS_PER_SECOND) *
                    TOD_NANOSECONDS_PER_MICROSECOND;
}
