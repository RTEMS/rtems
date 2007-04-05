/*
 *  adjime() function - required by NTP
 *
 *  I am unaware of the history behind the definition of this service
 *  and don't know if its behavior is covered by any standard. --joel
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
#include <sys/time.h>

#include <rtems/system.h>
#include <rtems/score/tod.h>
#include <rtems/posix/time.h>

static long __adjustment = 0;

int  adjtime ( struct timeval *delta, struct timeval *olddelta )
{
  struct timespec ts;

  if ( olddelta ) {
    olddelta->tv_sec  = __adjustment / TOD_MICROSECONDS_PER_SECOND;
    olddelta->tv_usec = __adjustment / TOD_MICROSECONDS_PER_SECOND;
  }

  if ( !delta )
    return -1;

  /* convert delta to microseconds */
  __adjustment  = (delta->tv_sec * TOD_MICROSECONDS_PER_SECOND);
  __adjustment += delta->tv_usec;

  /* too small to account for */
  if ( __adjustment < _TOD_Microseconds_per_tick )
    return 0;

  /* Grab the current TOD */
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
  return 0;
}
