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

#if defined(USE_USER_SIGNALS_PROCESS)
  #define TEST_NAME                "PSXSIGNAL 3"
  #define TEST_STRING              "User Signals to Process"
  #define SIGNAL_ONE               SIGUSR1
  #define SIGNAL_TWO               SIGUSR2
  #define SEND_SIGNAL(_sig)        kill( getpid(), _sig )
  #define TO_PROCESS

#elif defined(USE_REAL_TIME_SIGNALS_PROCESS)
  #define TEST_NAME                "PSXSIGNAL 4"
  #define TEST_STRING              "Real-Time Signals to Process"
  #define SIGNAL_ONE               SIGRTMIN
  #define SIGNAL_TWO               SIGRTMAX
  #define SEND_SIGNAL(_sig)        kill( getpid(), _sig )
  #define TO_PROCESS

#else
  #error "Test Mode not defined"
#endif

#include <pmacros.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

const char rtems_test_name[] = TEST_NAME;

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Test_Thread(void *arg);
void Signal_handler(int signo, siginfo_t *info, void *arg);
const char *signal_name(int signo);

volatile bool      Signal_occurred;
volatile pthread_t Signal_thread;

static void block_all_signals(void)
{
  int               sc;
  sigset_t          mask;

  sc = sigfillset( &mask );
  rtems_test_assert( !sc );

  sc = pthread_sigmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !sc );
}

void Signal_handler(
  int        signo,
  siginfo_t *info,
  void      *arg
)
{
  (void) signo;
  (void) info;
  (void) arg;

  Signal_occurred = true;
  Signal_thread   = pthread_self();
}

const char *signal_name(int signo)
{
  if (signo == SIGUSR1)
    return "SIGUSR1";
  if (signo == SIGUSR2)
    return "SIGUSR2";
  if (signo == SIGRTMIN)
    return "SIGRTMIN";
  if (signo == SIGRTMAX)
    return "SIGRTMAX";
  return "unknown-signal";
}

void *Test_Thread(void *arg)
{
  bool        blocked = *((bool *)arg);
  const char *name;
  int         sc;
  sigset_t    mask;
  sigset_t    wait_mask;
  siginfo_t   info;

  if ( blocked )
    name = "SignalBlocked";
  else
    name = "SignalNotBlocked";

  /* build unblocked mask */
  sc = sigemptyset( &mask );
  rtems_test_assert( !sc );

  printf( "%s - Unblock %s\n", name, signal_name(SIGNAL_ONE) );
  sc = sigaddset( &mask, SIGNAL_ONE );
  rtems_test_assert( !sc );

  if ( !blocked ) {
    printf( "%s - Unblock %s\n", name, signal_name(SIGNAL_TWO) );
    sc = sigaddset( &mask, SIGNAL_TWO );
    rtems_test_assert( !sc );
  }

  /* unblocked signals */
  sc = pthread_sigmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !sc );

  /* build wait mask */
  sc = sigemptyset( &wait_mask );
  rtems_test_assert( !sc );

  sc = sigaddset( &wait_mask, SIGNAL_ONE );
  rtems_test_assert( !sc );

  /* wait for a signal */
  memset( &info, 0, sizeof(info) );

  printf( "%s - Wait for %s unblocked\n", name, signal_name(SIGNAL_ONE) );
  sigwaitinfo( &wait_mask, &info );
  rtems_test_assert( !sc );

  printf( "%s - siginfo.si_signo=%d\n", name, info.si_signo );
  printf( "%s - siginfo.si_code=%d\n", name, info.si_code );
  /* FIXME: Instead of casting to (uintptr_t) and using PRIxPTR, we
   * likely should use %p. However, this would render this test's
   * behavior non-deterministic, because %p's behavior is
   * "implementation defined" */
  printf(
    "%s - siginfo.si_value=0x%08" PRIxPTR "\n",
    name,
    (uintptr_t) info.si_value.sival_ptr
  );

  rtems_test_assert( info.si_signo == SIGNAL_TWO );
  rtems_test_assert( info.si_code == SI_USER );

  printf( "%s - exiting\n", name );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  int                 sc;
  pthread_t           id;
  struct sigaction    act;
  bool                trueArg = true;
  bool                falseArg = false;
  struct timespec     delay_request;

  TEST_BEGIN();
  puts( "Init - Variation is: " TEST_STRING );

  block_all_signals();

  Signal_occurred = false;

  act.sa_handler = NULL;
  act.sa_sigaction = Signal_handler;
  act.sa_flags   = SA_SIGINFO;
  sigaction( SIGNAL_ONE, &act, NULL );
  sigaction( SIGNAL_TWO, &act, NULL );

  /* create threads */
  sc = pthread_create( &id, NULL, Test_Thread, &falseArg );
  rtems_test_assert( !sc );

  sc = pthread_create( &id, NULL, Test_Thread, &trueArg );
  rtems_test_assert( !sc );

  puts( "Init - sleep - let threads settle - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 5 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - sleep - SignalBlocked thread settle - OK" );
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  printf( "Init - sending %s - deliver to one thread\n",
          signal_name(SIGNAL_TWO));
  sc =  SEND_SIGNAL( SIGNAL_TWO );
  rtems_test_assert( !sc );

  printf( "Init - sending %s - deliver to other thread\n",
          signal_name(SIGNAL_TWO));
  sc =  SEND_SIGNAL( SIGNAL_TWO );
  rtems_test_assert( !sc );

  #if defined(TO_PROCESS)
    printf( "Init - sending %s - expect EAGAIN\n", signal_name(SIGNAL_TWO) );
    sc =  SEND_SIGNAL( SIGNAL_TWO );
    rtems_test_assert( sc == -1 );
    rtems_test_assert( errno == EAGAIN );
  #endif

  puts( "Init - sleep - let thread report if it unblocked - OK" );
  usleep(500000);

  /* we are just sigwait'ing the signal, not delivering it */
  rtems_test_assert( Signal_occurred == true );

  TEST_END();
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS        3
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
