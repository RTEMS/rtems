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
 *  _POSIX_Interval_to_timespec
 */
 
void _POSIX_Interval_to_timespec(
  Watchdog_Interval  ticks,
  struct timespec   *time
)
{
  unsigned32  usecs;

  usecs = ticks * _TOD_Microseconds_per_tick;

  time->tv_sec  = usecs / TOD_MICROSECONDS_PER_SECOND;
  time->tv_nsec = (usecs % TOD_MICROSECONDS_PER_SECOND) * 
                    TOD_NANOSECONDS_PER_MICROSECOND;
}
