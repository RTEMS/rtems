/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/seterr.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_Timespec_to_interval
 */

Watchdog_Interval _POSIX_Timespec_to_interval(
  const struct timespec *time
)
{
  Watchdog_Interval  ticks;

  ticks  = (time->tv_sec * TOD_MICROSECONDS_PER_SECOND) / 
             _TOD_Microseconds_per_tick;

  ticks += (time->tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND) / 
             _TOD_Microseconds_per_tick;

  if (ticks)
    return ticks;

  return 1;
}
