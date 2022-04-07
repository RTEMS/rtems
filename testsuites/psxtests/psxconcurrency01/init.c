/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2016
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
#include <pthread.h>
#include <errno.h>

const char rtems_test_name[] = "PSXCONCURRENCY01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int  sc;

  TEST_BEGIN();

  puts( "Init: pthread_getconcurrency - initial value of 0" );
  sc = pthread_getconcurrency();
  rtems_test_assert( sc == 0 );

  puts( "Init: pthread_setconcurrency - set value to 2" );
  sc = pthread_setconcurrency( 2 );
  rtems_test_assert( sc == 0 );

  puts( "Init: pthread_getconcurrency - confirm new value of 2" );
  sc = pthread_getconcurrency();
  rtems_test_assert( sc == 2 );

  TEST_END();
  rtems_test_exit(0);
}


/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
