#if !defined(RTEMS_UNIX)
/*
 *  RTEMS gettimeofday Implementation
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>

#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <time.h>
#include <sys/time.h>

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

  if ( tzp ) {
    tzp->tz_minuteswest = 0;  /* at UTC */
    tzp->tz_dsttime = 0;      /* no daylight savings */
#if 0
  tzp->minuteswest = timezone / 60; /* from seconds to minutes */
  tzp->dsttime = daylight;
#endif
  }
  return 0;
}

#if defined(RTEMS_NEWLIB) 

#if 0
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
#endif

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
