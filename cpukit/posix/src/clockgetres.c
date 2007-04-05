/*
 *  $Id$
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
    case CLOCK_PROCESS_CPUTIME:
    case CLOCK_THREAD_CPUTIME:
      if ( res ) {
        res->tv_sec = _TOD_Microseconds_per_tick / 1000000;
        res->tv_nsec = _TOD_Microseconds_per_tick * 1000;
        /* _TOD_From_ticks( _TOD_Microseconds_per_tick, res );  */
      }
      break;

    default:
      rtems_set_errno_and_return_minus_one( EINVAL );

  }
  return 0;
}
