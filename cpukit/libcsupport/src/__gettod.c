#if !defined(RTEMS_UNIX)
/*
 *  RTEMS gettimeofday Implementation
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>

#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <sys/time.h>
#include <time.h>

#include <errno.h>
#include <assert.h>

/*
 *  NOTE:  The solaris gettimeofday does not have a second parameter.
 */

int gettimeofday(
  struct timeval  *tp,
  struct timezone *tzp
)
{
  rtems_status_code      status;
  rtems_clock_time_value time;

  if ( !tp ) {
    errno = EFAULT;
    return -1;
  }

  /* "POSIX" does not seem to allow for not having a TOD */
  status = rtems_clock_get( RTEMS_CLOCK_GET_TIME_VALUE, &time );
  if ( status != RTEMS_SUCCESSFUL ) {
    assert( 0 );
    return -1;
  }

  tp->tv_sec  = time.seconds;
  tp->tv_usec = time.microseconds;

  /*
   * newlib does not have timezone and daylight savings time
   * yet.  When it does this needs to be fixed.
   */

#if 0
  if ( tzp ) {
    tzp->tz_minuteswest = 0;  /* at UTC */
    tzp->tz_dsttime = 0;      /* no daylight savings */
  tzp->minuteswest = timezone / 60; /* from seconds to minutes */
  tzp->dsttime = daylight;
  }
#endif
  return 0;
}

#if defined(RTEMS_NEWLIB) 

/*
 *  "Reentrant" version
 */

int _gettimeofday_r(
  struct _reent   *ignored_reentrancy_stuff,
  struct timeval  *tp,
  struct timezone *tzp
)
{
  return gettimeofday( tp, tzp );
}

/*
 *  "System call" version 
 */

int _gettimeofday(
  struct timeval  *tp,
  struct timezone *tzp
)
{
  return gettimeofday( tp, tzp );
}

#endif /* defined(RTEMS_NEWLIB) */

#endif
