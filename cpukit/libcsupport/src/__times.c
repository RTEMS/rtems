/*
 *  times() - POSIX 1003.1b 4.5.2 - Get Process Times
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <sys/times.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <rtems/seterr.h>
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timestamp.h>
#endif

clock_t _times(
   struct tms  *ptms
)
{
  rtems_interval ticks;

  if ( !ptms )
    rtems_set_errno_and_return_minus_one( EFAULT );

  /*
   *  This call does not depend on TOD being initialized and can't fail.
   */

  ticks = rtems_clock_get_ticks_since_boot();

  /*
   *  RTEMS technically has no notion of system versus user time
   *  since there is no separation of OS from application tasks.
   *  But we can at least make a distinction between the number
   *  of ticks since boot and the number of ticks executed by this
   *  this thread.
   */

  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    {
      Timestamp_Control per_tick;
      uint32_t          ticks;
      uint32_t          fractional_ticks;

      _Timestamp_Set(
        &per_tick,
        rtems_configuration_get_microseconds_per_tick() /
            TOD_MICROSECONDS_PER_SECOND,
        (rtems_configuration_get_nanoseconds_per_tick() %
            TOD_NANOSECONDS_PER_SECOND)
      );

      _Timestamp_Divide(
        &_Thread_Executing->cpu_time_used,
        &per_tick,
        &ticks,
        &fractional_ticks
      );
      ptms->tms_utime = ticks;
    }
  #else
    ptms->tms_utime  = _Thread_Executing->cpu_time_used;
  #endif
  ptms->tms_stime  = ticks;
  ptms->tms_cutime = 0;
  ptms->tms_cstime = 0;

  return ticks;
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
   struct _reent *ptr __attribute__((unused)),
   struct tms  *ptms
)
{
  return _times( ptms );
}
#endif
