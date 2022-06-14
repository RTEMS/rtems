/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*
 *  Needed to get the prototype for this libc helper method
 */
#define _LIBC

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
  cpu_time_used =
    _Thread_Get_CPU_time_used_after_last_reset( _Thread_Get_executing() );
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
