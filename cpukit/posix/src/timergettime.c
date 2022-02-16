/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function Fetches State of POSIX Per-Process Timers
 */

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 *
 *  COPYRIGHT (c) 1989-2008.
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

#include <time.h>
#include <errno.h>

#include <rtems/posix/timerimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/seterr.h>
#include <rtems/timespec.h>

/*
 *          - When a timer is initialized, the value of the time in
 *            that moment is stored.
 *          - When this function is called, it returns the difference
 *            between the current time and the initialization time.
 */

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
)
{
  POSIX_Timer_Control *ptimer;
  ISR_lock_Context lock_context;
  Per_CPU_Control *cpu;
  struct timespec now;
  struct timespec expire;
  struct timespec result;

  if ( !value )
    rtems_set_errno_and_return_minus_one( EINVAL );

  ptimer = _POSIX_Timer_Get( timerid, &lock_context );
  if ( ptimer == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  cpu = _POSIX_Timer_Acquire_critical( ptimer, &lock_context );
  rtems_timespec_from_ticks( ptimer->Timer.expire, &expire );

  if ( ptimer->clock_type == CLOCK_MONOTONIC ) {
  _Timecounter_Nanouptime(&now);
  } else  if (ptimer->clock_type == CLOCK_REALTIME) {
    _TOD_Get(&now);
  } else {
    _POSIX_Timer_Release( cpu, &lock_context );
    rtems_set_errno_and_return_minus_one( EINVAL );
  }


 if ( rtems_timespec_less_than( &now, &expire ) ) {
      rtems_timespec_subtract( &now, &expire, &result );
  } else {
    result.tv_nsec = 0;
    result.tv_sec  = 0;
  }

  value->it_value = result;
  value->it_interval = ptimer->timer_data.it_interval;

  _POSIX_Timer_Release( cpu, &lock_context );
  return 0;
}