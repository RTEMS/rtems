/*
 *  adjime() function
 *
 *  This method was initially added as part of porting NTP to RTEMS.
 *  It is a BSD compatability function and now is available on
 *  GNU/Linux.
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

#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/seterr.h>
#include <rtems/score/tod.h>
#include <rtems/posix/time.h>
#include <rtems/score/thread.h>
#include <rtems/score/timespec.h>

/*
 *  At one point there was a static variable named adjustment
 *  used by this implementation.  I don't see any reason for it
 *  to be here based upon the GNU/Linux documentation.
 */

int  adjtime(
  const struct timeval *delta,
  struct timeval *olddelta
)
{
  struct timespec ts;
  long   adjustment;

  /*
   * Simple validations
   */
  if ( !delta )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( delta->tv_usec >= TOD_MICROSECONDS_PER_SECOND )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( olddelta ) {
    olddelta->tv_sec  = 0;
    olddelta->tv_usec = 0;
  }

  /* convert delta to microseconds */
  adjustment  = (delta->tv_sec * TOD_MICROSECONDS_PER_SECOND);
  adjustment += delta->tv_usec;

  /* too small to account for */
  if ( adjustment < rtems_configuration_get_microseconds_per_tick() )
    return 0;

  /*
   * This prevents context switches while we are adjusting the TOD
   */

  _Thread_Disable_dispatch();

    _TOD_Get( &ts );

    ts.tv_sec  += delta->tv_sec;
    ts.tv_nsec += delta->tv_usec * TOD_NANOSECONDS_PER_MICROSECOND;

    /* if adjustment is too much positive */
    while ( ts.tv_nsec >= TOD_NANOSECONDS_PER_SECOND ) {
      ts.tv_nsec -= TOD_NANOSECONDS_PER_SECOND;
      ts.tv_sec++;
    }

    /* if adjustment is too much negative */
    while ( ts.tv_nsec <= (-1 * TOD_NANOSECONDS_PER_SECOND) ) {
      ts.tv_nsec += TOD_NANOSECONDS_PER_SECOND;
      ts.tv_sec--;
    }

    _TOD_Set( &ts );

  _Thread_Enable_dispatch();

  /* set the user's output */
  if ( olddelta )
    *olddelta = *delta;

  return 0;
}
