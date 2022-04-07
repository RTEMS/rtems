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

#include <pmacros.h>
#include <sys/time.h>
#include <errno.h>

const char rtems_test_name[] = "PSXITIMER";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int              status;
  struct itimerval itimer;
  struct itimerval otimer;

  TEST_BEGIN();

  /* test getitimer stub */
  puts( "getitimer -- bad which - EINVAL " );
  status = getitimer( 1234, &itimer );
  rtems_test_assert( status == -1 && errno == EINVAL );

  puts( "getitimer -- NULL pointer - EFAULT " );
  status = getitimer( ITIMER_REAL, NULL );
  rtems_test_assert( status == -1 && errno == EFAULT );

  puts( "getitimer -- ITIMER_REAL - ENOSYS " );
  status = getitimer( ITIMER_REAL, &itimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "getitimer -- ITIMER_VIRTUAL - ENOSYS " );
  status = getitimer( ITIMER_VIRTUAL, &itimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "getitimer -- ITIMER_PROF - ENOSYS " );
  status = getitimer( ITIMER_PROF, &itimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  /* test setitimer stub */
  puts( "setitimer -- bad which - EINVAL " );
  status = setitimer( 1234, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == EINVAL );

  puts( "setitimer -- NULL value pointer - EFAULT " );
  status = setitimer( ITIMER_REAL, NULL, &otimer );
  rtems_test_assert( status == -1 && errno == EFAULT );

  puts( "setitimer -- NULL value pointer - EFAULT " );
  status = setitimer( ITIMER_REAL, &itimer, NULL );
  rtems_test_assert( status == -1 && errno == EFAULT );

  puts( "setitimer -- ITIMER_REAL - ENOSYS " );
  status = setitimer( ITIMER_REAL, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "setitimer -- ITIMER_VIRTUAL - ENOSYS " );
  status = setitimer( ITIMER_VIRTUAL, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  puts( "setitimer -- ITIMER_PROF - ENOSYS " );
  status = setitimer( ITIMER_PROF, &itimer, &otimer );
  rtems_test_assert( status == -1 && errno == ENOSYS );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
