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
#include <errno.h>
#include <pthread.h>
#include <sched.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/stackimpl.h>

const char rtems_test_name[] = "PSXSTACK 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Test_Thread(void *argument);

void *Stack_Low;
void *Stack_High;

void *Test_Thread(void *arg)
{
  #if defined(__GNUC__)
    void *sp = __builtin_frame_address(0);

    #if 0
      printf( "Stack(%p - %p) and sp=%p\n", Stack_Low, Stack_High, sp );
    #endif

    if ( sp >= Stack_Low && sp <= Stack_High )
      puts( "Test_Thread - running on user provided stack - OK" );
    else {
      puts( "Test_Thread - ERROR running on other stack" );
      rtems_test_exit(0);
    }
  #else
      puts( "Test_Thread - no way to get stack pointer and verify" );
  #endif
  puts( "Test_Thread - delete self" );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int                 sc;
  pthread_t           id;
  pthread_attr_t      attr;
  struct timespec     delay_request;

  TEST_BEGIN();

  puts( "Init - Allocate stack from heap" );
  Stack_Low = malloc(PTHREAD_MINIMUM_STACK_SIZE);
  rtems_test_assert( Stack_Low );
  Stack_High = Stack_Low + PTHREAD_MINIMUM_STACK_SIZE;

  puts( "Init - Initialize thread attribute for user provided stack" );
  sc = pthread_attr_init( &attr );
  rtems_test_assert( !sc );

  sc = pthread_attr_setstackaddr( &attr, Stack_Low );
  rtems_test_assert( !sc );

  sc = pthread_attr_setstacksize( &attr, PTHREAD_MINIMUM_STACK_SIZE );
  rtems_test_assert( !sc );

  /* create threads */
  sc = pthread_create( &id, &attr, Test_Thread, NULL );
  rtems_test_assert( !sc );

  puts( "Init - let other thread run" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 5 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  TEST_END();
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS        2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
