/*
 *  adjustime() function - required by NTP
 *
 *  I am unaware of the history behind the definition of this service
 *  and don't know if its behavior is covered by any standard. --joel 
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

  __adjustment = (delta->tv_sec * TOD_MICROSECONDS_PER_SECOND) + delta->tv_usec;
  /* too small to account for */
  if ( __adjustment < _TOD_Microseconds_per_tick )
    return 0;
   
  clock_gettime( CLOCK_REALTIME, &ts );

  ts.tv_sec  += (__adjustment / TOD_MICROSECONDS_PER_SECOND);
  ts.tv_nsec += (__adjustment % TOD_MICROSECONDS_PER_SECOND) * 
                       TOD_NANOSECONDS_PER_MICROSECOND;

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

  clock_settime( CLOCK_REALTIME, &ts );
  return 0;
}
