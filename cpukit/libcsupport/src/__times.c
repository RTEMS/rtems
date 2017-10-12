/**
 *  @file
 *
 *  @brief Get Process Times
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/*
 *  Needed to get the prototype for this newlib helper method
 */
#define _COMPILING_NEWLIB

#include <rtems.h>

#include <sys/times.h>
#include <sys/time.h>

#include <string.h>
#include <time.h>

#include <rtems/seterr.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/threadimpl.h>

/**
 *  POSIX 1003.1b 4.5.2 - Get Process Times
 */
clock_t _times(
   struct tms  *ptms
)
{
  uint32_t   tick_interval;
  sbintime_t uptime;
  sbintime_t cpu_time_used;

  if ( !ptms )
    rtems_set_errno_and_return_minus_one( EFAULT );

  tick_interval = (uint32_t)
    (SBT_1US * rtems_configuration_get_microseconds_per_tick());

  ptms = memset( ptms, 0, sizeof( *ptms ) );

  _TOD_Get_zero_based_uptime( &uptime );
  ptms->tms_stime = ((clock_t) uptime) / tick_interval;

  /*
   *  RTEMS technically has no notion of system versus user time
   *  since there is no separation of OS from application tasks.
   *  But we can at least make a distinction between the number
   *  of ticks since boot and the number of ticks executed by this
   *  this thread.
   */
  _Thread_Get_CPU_time_used( _Thread_Get_executing(), &cpu_time_used );
  ptms->tms_utime = ((clock_t) cpu_time_used) / tick_interval;

  return ptms->tms_stime;
}

/**
 *  times() system call wrapper for _times() above.
 */
clock_t times(
   struct tms  *ptms
)
{
  return _times( ptms );
}

#if defined(RTEMS_NEWLIB)

#include <reent.h>

/**
 *  This is the Newlib dependent reentrant version of times().
 */
clock_t _times_r(
   struct _reent *ptr RTEMS_UNUSED,
   struct tms  *ptms
)
{
  return _times( ptms );
}
#endif
