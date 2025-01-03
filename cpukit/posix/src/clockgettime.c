/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief This source file contains the implementation of clock_gettime().
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
 *
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/cpuuseimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/timestampimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/seterr.h>

/*
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_gettime(
  clockid_t        clock_id,
  struct timespec *tp
)
{
  if ( !tp )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( clock_id == CLOCK_REALTIME ) {
    _TOD_Get(tp);
    return 0;
  }

#ifdef CLOCK_MONOTONIC
  if ( clock_id == CLOCK_MONOTONIC ) {
    _Timecounter_Nanouptime( tp );
    return 0;
  }
#endif

#ifdef _POSIX_CPUTIME
  if ( clock_id == CLOCK_PROCESS_CPUTIME_ID ) {
    Timestamp_Control uptime;
    Timestamp_Control last_reset;
    Timestamp_Control process_time;

    last_reset = CPU_usage_Uptime_at_last_reset;
    _TOD_Get_uptime( &uptime );
    _Timestamp_Subtract( &last_reset, &uptime, &process_time );
    _Timestamp_To_timespec( &process_time, tp );
    return 0;
  }
#endif

#ifdef _POSIX_THREAD_CPUTIME
  if ( clock_id == CLOCK_THREAD_CPUTIME_ID ) {
    Timestamp_Control used;

    used = _Thread_Get_CPU_time_used_after_last_reset(
      _Thread_Get_executing()
    );
    _Timestamp_To_timespec( &used, tp );
    return 0;
  }
#endif

  rtems_set_errno_and_return_minus_one( EINVAL );
}
