/*
 *  gettimeofday() - SVR4 and BSD4.3 extension required by Newlib
 *
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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#if defined(RTEMS_NEWLIB)
#include <sys/time.h>
#include <errno.h>
#include <rtems.h>
#include <rtems/seterr.h>

#if defined(RTEMS_NEWLIB) && !defined(HAVE_GETTIMEOFDAY)
/*
 *  NOTE:  The solaris gettimeofday does not have a second parameter.
 */
int gettimeofday(
  struct timeval  *tp,
  void * __tz __attribute__((unused))
)
{
  /* struct timezone* tzp = (struct timezone*) __tz; */
  if ( !tp )
    rtems_set_errno_and_return_minus_one( EFAULT );

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
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE__GETTIMEOFDAY_R)

#include <sys/reent.h>

/*
 *  "Reentrant" version
 */
int _gettimeofday_r(
  struct _reent   *ignored_reentrancy_stuff __attribute__((unused)),
  struct timeval  *tp,
  struct timezone *tzp
)
{
  return gettimeofday( tp, tzp );
}
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE__GETTIMEOFDAY)
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
#endif

#endif /* defined(RTEMS_NEWLIB) */
