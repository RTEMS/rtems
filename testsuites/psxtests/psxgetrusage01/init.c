/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#include <tmacros.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXGETRUSAGE 1";

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

rtems_task Init(
  rtems_task_argument argument
)
{
  int           sc;
  struct rusage usage;

  TEST_BEGIN();

  puts( "getrusage( RUSAGE_SELF, NULL ) -- EFAULT" );
  sc = getrusage( RUSAGE_SELF, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "getrusage( RUSAGE_CHILDREN, &usage ) -- ENOSYS" );
  sc = getrusage( RUSAGE_CHILDREN, &usage );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOSYS );

  puts( "getrusage( 77, &usage ) -- EINVAL" );
  sc = getrusage( 77, &usage );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Consume CPU long enough to have non-zero usage" );
  rtems_test_spin_for_ticks( 5 );
  
  puts( "getrusage( RUSAGE_SELF, &usage ) -- EINVAL" );
  sc = getrusage( RUSAGE_SELF, &usage );
  rtems_test_assert( sc == 0 );

  /* CPU usage is non-zero */
  rtems_test_assert( usage.ru_utime.tv_sec == 0 );
  rtems_test_assert( usage.ru_utime.tv_usec != 0 );

  /* System and user time is the same */
  rtems_test_assert( usage.ru_utime.tv_sec == usage.ru_stime.tv_sec );
  rtems_test_assert( usage.ru_utime.tv_usec == usage.ru_stime.tv_usec );
  
  TEST_END();

  rtems_test_exit(0);
}
