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

#if HAVE_CONFIG_H
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
  Timestamp_Control  delta_as_timestamp;
  Timestamp_Control  tod_as_timestamp;
  Timestamp_Control *tod_as_timestamp_ptr;

  /*
   * Simple validations
   */
  if ( !delta )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   * Currently, RTEMS does the adjustment in one movement.
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
   * convert delta timeval to internal timestamp
   */
  _Timestamp_Set( &delta_as_timestamp, delta->tv_sec, delta->tv_usec * 1000 );

  /*
   * This prevents context switches while we are adjusting the TOD
   */

  _Thread_Disable_dispatch();

    tod_as_timestamp_ptr =
      _TOD_Get_with_nanoseconds( &tod_as_timestamp, &_TOD.now );


    _Timestamp_Add_to( tod_as_timestamp_ptr, &delta_as_timestamp );

    _TOD_Set_with_timestamp( tod_as_timestamp_ptr );

  _Thread_Enable_dispatch();

  return 0;
}
