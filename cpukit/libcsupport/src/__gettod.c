/*
 *  gettimeofday() - SVR4 and BSD4.3 extension required by Newlib
 *
 *  COPYRIGHT (c) 1989-1999.
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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <rtems.h>

#if !defined(RTEMS_UNIX)
#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <sys/time.h>
#include <time.h>

#include <errno.h>
#include <assert.h>

/*
 *  Seconds from January 1, 1970 to January 1, 1988.  Used to account for
 *  differences between POSIX API and RTEMS core.
 */

#define POSIX_TIME_SECONDS_1970_THROUGH_1988 \
  (((1987 - 1970 + 1)  * TOD_SECONDS_PER_NON_LEAP_YEAR) + \
  (4 * TOD_SECONDS_PER_DAY))

/*
 *  NOTE:  The solaris gettimeofday does not have a second parameter.
 */

int gettimeofday(
  struct timeval  *tp,
  struct timezone *tzp
)
{
  rtems_interrupt_level level;
  rtems_unsigned32      seconds;
  rtems_unsigned32      microseconds;

  if ( !tp ) {
    errno = EFAULT;
    return -1;
  }

  /*
   *  POSIX does not seem to allow for not having a TOD so we just
   *  grab the time of day.
   *
   *  NOTE: XXX this routine should really be in the executive proper.
   */

  rtems_interrupt_disable(level);
    seconds      = _TOD_Seconds_since_epoch;
    microseconds = _TOD_Current.ticks;
  rtems_interrupt_enable(level);

  tp->tv_sec  = seconds + POSIX_TIME_SECONDS_1970_THROUGH_1988;
  tp->tv_usec = microseconds * _TOD_Microseconds_per_tick;

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
