/*
 *  RTEMS _times Implementation
 *
 *
 *  COPYRIGHT (c) 1989-1997.
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

#include <sys/times.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>

clock_t _times(
   struct tms  *ptms
)
{
  rtems_status_code      status;
  rtems_interval         ticks_since_boot;

  if ( !ptms ) {
    errno = EFAULT;
    return -1;
  }

  /* "POSIX" does not seem to allow for not having a TOD */
  status = rtems_clock_get(
    RTEMS_CLOCK_GET_TICKS_SINCE_BOOT,
    &ticks_since_boot
  );
  if ( status != RTEMS_SUCCESSFUL ) {
    assert( 0 );
    return -1;
  }

  /*
   *  RTEMS has no notion of system versus user time and does
   *  not (as of 3.2.0) keep track of CPU usage on a per task basis.
   */

  ptms->tms_utime  = ticks_since_boot;
  ptms->tms_stime  = 0;
  ptms->tms_cutime = 0;
  ptms->tms_cstime = 0;

  return 0;
}

clock_t times(
   struct tms  *ptms
)
{
  return _times( ptms );
}

#if defined(RTEMS_NEWLIB)
clock_t _times_r(
   struct _reent *ptr,
   struct tms  *ptms
)
{
  return _times(ptms);
}
#endif
