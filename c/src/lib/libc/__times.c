/*
 *  times() - POSIX 1003.1b 4.5.2 - Get Process Times
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <sys/times.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>

clock_t _times(
   struct tms  *ptms
)
{
  rtems_status_code  status;
  rtems_interval     ticks;

  if ( !ptms ) {
    errno = EFAULT;
    return -1;
  }

  /* "POSIX" does not seem to allow for not having a TOD */
  status = rtems_clock_get( RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &ticks );
  if ( status != RTEMS_SUCCESSFUL ) {
    assert( 0 );
    return -1;
  }

  /*
   *  RTEMS has no notion of system versus user time and although
   *  a way to keep track of per task CPU usage was added since
   *  3.6.0, this routine does not utilize it yet.
   */

  ptms->tms_utime  = ticks;
  ptms->tms_stime  = 0;
  ptms->tms_cutime = 0;
  ptms->tms_cstime = 0;

  return 0;
}

/*
 *  times()
 *
 *  times() system call wrapper for _times() above.
 */

clock_t times(
   struct tms  *ptms
)
{
  return _times( ptms );
}

/*
 *  _times_r
 *
 *  This is the Newlib dependent reentrant version of times().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

clock_t _times_r(
   struct _reent *ptr,
   struct tms  *ptms
)
{
  return _times( ptms );
}
#endif
