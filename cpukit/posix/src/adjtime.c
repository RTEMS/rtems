/**
 *  @file
 *
 *  @brief Adjust the Time to Synchronize the System Clock
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _BSD_SOURCE
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include <rtems/score/timespec.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/todimpl.h>
#include <rtems/config.h>
#include <rtems/seterr.h>

/**
 * This method was initially added as part of porting NTP to RTEMS.
 * It is a BSD compatability function and now is available on
 * GNU/Linux.
 * 
 * At one point there was a static variable named adjustment
 * used by this implementation.  I don't see any reason for it
 * to be here based upon the GNU/Linux documentation.
 */
int adjtime(
  const struct timeval *delta,
  struct timeval       *olddelta
)
{
  struct timespec delta_as_timespec;

  /*
   * Simple validations
   */
  if ( !delta )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( delta->tv_usec >= TOD_MICROSECONDS_PER_SECOND )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   * An adjustment of zero is pretty easy.
   */
  if ( delta->tv_sec == 0 && delta->tv_usec == 0 )
    return 0;

  /*
   * Currently, RTEMS does the adjustment in one movement so there
   * is no way an adjustment was currently underway.
   *
   * Given interest, requirements, and sponsorship, a future
   * enhancement would be to adjust the time in smaller increments
   * at each clock tick. Until then, there is no outstanding
   * adjustment.
   */
  if ( olddelta ) {
    olddelta->tv_sec  = 0;
    olddelta->tv_usec = 0;
  }

  /*
   * convert delta timeval to timespec
   */
  delta_as_timespec.tv_sec = delta->tv_sec;
  delta_as_timespec.tv_nsec = delta->tv_usec * 1000;

  /*
   * Now apply the adjustment
   */
  _TOD_Adjust( &delta_as_timespec );

  return 0;
}
