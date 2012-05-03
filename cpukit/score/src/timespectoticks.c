/**
 *  @file  score/src/timespectoticks.c
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

#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/**
 *
 *  This routines converts a timespec to the corresponding number of ticks.
 */

uint32_t _Timespec_To_ticks(
  const struct timespec *time
)
{
  uint32_t  ticks;
  uint32_t  nanoseconds_per_tick;

  if ( (time->tv_sec == 0) && (time->tv_nsec == 0) )
    return 0;

  /**
   *  We should ensure the ticks not be truncated by integer division.  We
   *  need to have it be greater than or equal to the requested time.  It
   *  should not be shorter.
   */
  ticks                 = time->tv_sec * TOD_TICKS_PER_SECOND;
  nanoseconds_per_tick  = rtems_configuration_get_nanoseconds_per_tick();
  ticks                += time->tv_nsec / nanoseconds_per_tick;

  if ( (time->tv_nsec % nanoseconds_per_tick) != 0 )
    ticks += 1;

  return ticks;
}
