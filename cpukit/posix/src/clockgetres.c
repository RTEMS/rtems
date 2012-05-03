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
#include <errno.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/seterr.h>

/*
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_getres(
  clockid_t        clock_id,
  struct timespec *res
)
{
  if ( !res )
    rtems_set_errno_and_return_minus_one( EINVAL );

  switch ( clock_id ) {

    /*
     *  All time in rtems is based on the same clock tick.
     */

    case CLOCK_REALTIME:
    case CLOCK_PROCESS_CPUTIME_ID:
    case CLOCK_THREAD_CPUTIME_ID:
      if ( res ) {
        res->tv_sec = rtems_configuration_get_microseconds_per_tick() /
            TOD_MICROSECONDS_PER_SECOND;
        res->tv_nsec = rtems_configuration_get_nanoseconds_per_tick();
      }
      break;

    default:
      rtems_set_errno_and_return_minus_one( EINVAL );

  }
  return 0;
}
