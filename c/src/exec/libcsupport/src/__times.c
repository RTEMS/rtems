/*
 *  RTEMS _times Implementation
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

