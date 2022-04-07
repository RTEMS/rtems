/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#include <tmacros.h>
#include "test_support.h"
#include <sys/times.h>
#include <errno.h>

const char rtems_test_name[] = "PSXTIMES 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
clock_t _times_r(struct _reent *ptr, struct tms  *ptms);
clock_t _times(struct tms  *ptms);

rtems_task Init(
  rtems_task_argument argument
)
{
  clock_t    start;
  clock_t    end;
  clock_t    now;
  clock_t    sc;
  clock_t    difference;
  struct tms start_tm;
  struct tms end_tm;
  int        interval = 5;

  TEST_BEGIN();

  puts( "times( NULL ) -- EFAULT" );
  sc = times( NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "_times_r( NULL, NULL ) -- EFAULT" );
  start = _times_r( NULL, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  while ( rtems_clock_get_ticks_since_boot() <= 2 )
    ;

  puts( "_times( &start_tm ) -- OK" );
  now = _times( &start_tm );
  rtems_test_assert( start != 0 );
  rtems_test_assert( now != 0 );
  
  rtems_test_spin_for_ticks( interval );

  puts( "_times( &end_tm ) -- OK" );
  end = _times( &end_tm );
  rtems_test_assert( end != 0 );
  
  puts( "Check various values" );
  difference = end - start;
  rtems_test_assert( difference >= interval );

  rtems_test_assert( end_tm.tms_utime - start_tm.tms_utime >= interval );
  rtems_test_assert( end_tm.tms_stime - start_tm.tms_stime >= interval );
  rtems_test_assert( end_tm.tms_cutime == 0 );
  rtems_test_assert( end_tm.tms_cstime == 0 );
  
  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
