/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <tmacros.h>
#include "test_support.h"
#include <pthread.h>
#include <errno.h>

const char rtems_test_name[] = "PSXSIGNAL 6";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void Handler(int signo);
void *TestThread(void *argument);

pthread_t       ThreadId;
pthread_cond_t  CondVarId;
pthread_mutex_t MutexId;

void Handler(
  int signo
)
{
}

void *TestThread(
  void *argument
)
{
  int               status;
  sigset_t          mask;
  struct sigaction  act;
  unsigned int      left;

  /* unblock SIGUSR1 */
  status = sigemptyset( &mask );
  rtems_test_assert(  !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert(  !status );

  puts( "Test: Unblock SIGUSR1" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert(  !status );

  /* install a signal handler for SIGUSR1 */
  act.sa_handler = Handler;
  act.sa_flags   = 0;
  sigaction( SIGUSR1, &act, NULL );

  status = pthread_mutex_lock( &MutexId );
  rtems_test_assert(  !status );

  /* interrupting condition wait returns 0 */
  puts( "Test: pthread_cond_wait - OK" );
  status = pthread_cond_wait( &CondVarId, &MutexId );
  rtems_test_assert( !status );
  puts( "Test: pthread_cond_wait - interrupted by signal" );

  left = sleep( 10 );
  printf( "Test: seconds left=%d\n", left );

  return NULL;
}

void *POSIX_Init(void *argument)
{
  int status;

  TEST_BEGIN();

  puts( "Init: pthread_cond_init - OK" );
  status = pthread_cond_init( &CondVarId, NULL );
  rtems_test_assert(  !status );

  puts( "Init: pthread_mutex_init - OK" );
  status = pthread_mutex_init( &MutexId, NULL );
  rtems_test_assert(  !status );

  puts( "Init: pthread_create - OK" );
  status = pthread_create( &ThreadId, NULL, TestThread, NULL );
  rtems_test_assert( !status );

  sleep( 1 );

     /* let TestThread run */

  puts( "Init: pthread_kill - SIGUSR to Test Thread - OK" );
  status = pthread_kill( ThreadId, SIGUSR1 );
  rtems_test_assert( !status );

  sleep( 2 );

     /* let TestThread run */

  puts( "Init: pthread_kill - SIGUSR to Test Thread - OK" );
  status = pthread_kill( ThreadId, SIGUSR1 );
  rtems_test_assert( !status );

  sleep( 1 );

     /* let TestThread run */



  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS             2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
