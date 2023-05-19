/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <sys/time.h>
#include <sys/timex.h>
#include <errno.h>
#include <string.h>

#include <rtems/test.h>
#include <rtems/test-info.h>

const char rtems_test_name[] = "SPNTP 1";

T_TEST_CASE( NTP )
{
  struct ntptimeval ntv;
  struct timex tx;
  int status;

  errno = 0;
  status = ntp_gettime( NULL );
  T_eq_int( status, -1 );
  T_eq_int( errno, EFAULT );

  memset( &ntv, 0xff, sizeof( ntv ) );
  status = ntp_gettime( &ntv );
  T_eq_int( status, TIME_ERROR );

  T_eq_i64( ntv.time.tv_sec, 567993600 );
  T_eq_long( ntv.time.tv_nsec, 0 );
  T_eq_long( ntv.maxerror, 500000 );
  T_eq_long( ntv.esterror, 500000 );
  T_eq_long( ntv.tai, 0 );
  T_eq_long( ntv.time_state, TIME_ERROR );

  errno = 0;
  status = ntp_adjtime( NULL );
  T_eq_int( status, -1 );
  T_eq_int( errno, EFAULT );

  memset( &tx, 0xff, sizeof( tx ) );
  tx.modes = 0;
  status = ntp_adjtime( &tx );
  T_eq_int( status, TIME_ERROR );
  T_eq_uint( tx.modes, 0 );
  T_eq_long( tx.offset, 0 );
  T_eq_long( tx.freq, 0 );
  T_eq_long( tx.maxerror, 500000 );
  T_eq_long( tx.esterror, 500000 );
  T_eq_int( tx.status, STA_UNSYNC );
  T_eq_long( tx.constant, 0 );
  T_eq_long( tx.precision, 0 );
  T_eq_long( tx.tolerance, 32500000 );
  T_eq_long( tx.ppsfreq, 0 );
  T_eq_long( tx.jitter, 0 );
  T_eq_int( tx.shift, 2 );
  T_eq_long( tx.stabil, 0 );
  T_eq_long( tx.jitcnt, 0 );
  T_eq_long( tx.calcnt, 0 );
  T_eq_long( tx.errcnt, 0 );
  T_eq_long( tx.stbcnt, 0 );

  status = adjtime( NULL,  NULL );
  T_eq_int( status, 0 );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
