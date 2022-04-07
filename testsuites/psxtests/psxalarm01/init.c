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
#include <signal.h>
#include <errno.h>

const char rtems_test_name[] = "PSXALARM 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

volatile int Signal_count;

static void Signal_handler(
  int signo
)
{
  Signal_count++;
  printf(
    "Signal: %d caught by 0x%" PRIxpthread_t " (%d)\n",
    signo,
    pthread_self(),
    Signal_count
  );
}

void *POSIX_Init(
  void *argument
)
{
  unsigned int      remaining;
  int               sc;
  struct sigaction  act;
  sigset_t          mask;
  struct timeval    delta;

  TEST_BEGIN();

  /* install a signal handler for SIGALRM and unblock it */

  sc = sigemptyset( &act.sa_mask );
  rtems_test_assert( !sc );

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;

  sigaction( SIGALRM, &act, NULL );

  sc = sigemptyset( &mask );
  rtems_test_assert( !sc );

  sc = sigaddset( &mask, SIGALRM );
  rtems_test_assert( !sc );

  puts( "Init: Unblock SIGALRM" );
  sc = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !sc );

  /* schedule the alarm */

  puts( "Init: Firing alarm in 1 second" );
  remaining = alarm( 1 );
  printf( "Init: %d seconds left on previous alarm\n", sc );
  rtems_test_assert( !sc );

  puts( "Init: Wait for alarm" );
  sleep( 2 );

  rtems_test_assert( Signal_count == 1 );

  puts( "Init: Cancel alarm" );
  remaining = alarm( 0 );
  printf( "Init: %d seconds left on previous alarm\n", remaining );
  rtems_test_assert( remaining == 0 );

  remaining = alarm( 1 );
  rtems_test_assert( remaining == 0 );

  delta.tv_sec = 2;
  delta.tv_usec = 0;
  sc = adjtime( &delta, NULL );
  rtems_test_assert( sc == 0 );

  rtems_test_assert( Signal_count == 1 );

  remaining = alarm( 0 );
  rtems_test_assert( remaining == 1 );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS        1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
        (RTEMS_MINIMUM_STACK_SIZE * 4)

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

