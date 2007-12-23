/*
 *  gettimeofday() - SVR4 and BSD4.3 extension required by Newlib
 *
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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <rtems.h>

#if !defined(RTEMS_UNIX)
#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <sys/time.h>
#include <time.h>

#include <errno.h>

/*
 *  NOTE:  The solaris gettimeofday does not have a second parameter.
 */

int gettimeofday(
  struct timeval  *tp,
  void * __tz
)
{
  /* struct timezone* tzp = (struct timezone*) __tz; */
  if ( !tp ) {
    errno = EFAULT;
    return -1;
  }

  /*
   *  POSIX does not seem to allow for not having a TOD so we just
   *  grab the time of day.
   */

  _TOD_Get_timeval( tp );

  /*
   *  Timezone information ignored by the OS proper.   Per email
   *  with Eric Norum, this is how GNU/Linux, Solaris, and MacOS X
   *  do it.  This puts us in good company.
   */

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
