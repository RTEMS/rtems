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
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

const char rtems_test_name[] = "PSXSIGNAL 2";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Test_Thread(void *arg);
void Signal_handler(int signo);
void Install_Signal_Handler(const char *task_name);

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
  int signo
)
{
  (void) signo;

  Signal_occurred = true;
  Signal_thread   = pthread_self();
}

void Install_Signal_Handler(
  const char *task_name
)
{
  int               sc;
  sigset_t          mask;

  sc = sigemptyset( &mask );
  rtems_test_assert( !sc );

  sc = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !sc );

  printf( "%s - Unblock SIGUSR1\n", task_name );
  sc = pthread_sigmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !sc );
}

/*

Tasks and actions, created in this order, all interested in SIGUSR1
  - 20     - interested, suspend?
  - 18     - interested, suspend?
  - 16     - interested, spins
  - 14     - interested, spins
  - 12     - interested, sleeps
  - 10     - interested, suspends
  - 8      - interested, sleeps

Order is critical because the algorithm works by thread index
*/

typedef enum {
  SUSPEND,
  SPIN,
  SLEEP
} Action_t;

const char *Actions[] = {
  "Suspends self",
  "Spins",
  "Sleeps"
};


typedef struct {
  int         priority;
  Action_t    action;
  const char *name;
} Test_t;

Test_t Threads[] = {
  {  8, SUSPEND,  "P8"  },  /* base priority */
  {  7, SUSPEND,  "P7"  },  /* lower priority -- no change */
  { 12, SUSPEND,  "P12" },  /* higher priority, blocked */
  { 12, SUSPEND,  "P12" },  /* equal priority, blocked */
  { 12, SLEEP,    "P12" },  /* equal priority, interruptible */
  { 12, SLEEP,    "P12" },  /* equal priority, interruptible */
  { 12, SPIN,     "P12" },  /* equal priority, ready */
  { 12, SPIN,     "P12" },  /* equal priority, ready -- no change */
  { -1, 0,        ""    },
};

void *Test_Thread(void *arg)
{
  (void) arg;

  Test_t *test = (Test_t *)arg;

  Install_Signal_Handler( test->name );

  printf( "%s - %s\n", test->name, Actions[test->action] );
  switch ( test->action ) {
    case SUSPEND:
      (void) rtems_task_suspend( RTEMS_SELF );
      break;
    case SPIN:
      while (1) ;
      break;
    case SLEEP:
      sleep( 30 );
      break;
  }

  printf( "%s - exiting\n", test->name );
  return NULL;

}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  int                 i;
  int                 sc;
  pthread_t           id;
  pthread_attr_t      attr;
  struct sched_param  param;
  Test_t             *test;
  struct sigaction    act;
  struct timespec     delay_request;

  TEST_BEGIN();

  Signal_occurred = false;

  block_all_signals();

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
  sigaction( SIGUSR1, &act, NULL );

  puts( "Init - Raise my priority" );
  sc = pthread_attr_init( &attr );
  rtems_test_assert( !sc );

  param.sched_priority = 30;
  sc = pthread_setschedparam( pthread_self(), SCHED_RR, &param );
  rtems_test_assert( !sc );

  for ( i=0, test=Threads ; test->priority != -1 ; i++, test++ ) {
    printf( "Init - Create thread %d, priority=%d\n", i, test->priority );
    sc = pthread_attr_init( &attr );
    rtems_test_assert( !sc );

    sc = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
    rtems_test_assert( !sc );

    sc = pthread_attr_setschedpolicy( &attr, SCHED_RR );
    rtems_test_assert( !sc );

    param.sched_priority = test->priority;
    sc = pthread_attr_setschedparam( &attr, &param );
    rtems_test_assert( !sc );

    sc = pthread_create( &id, &attr, Test_Thread, test );
    rtems_test_assert( !sc );

    puts( "Init - sleep - let thread settle - OK" );
    delay_request.tv_sec = 0;
    delay_request.tv_nsec = 50000000;
    sc = nanosleep( &delay_request, NULL );
    rtems_test_assert( !sc );
  }

  puts( "Init - sending SIGUSR1" );
  sc =  kill( getpid(), SIGUSR1 );
  rtems_test_assert( !sc );

  /* we are just scheduling the signal, not delivering it */
  rtems_test_assert( Signal_occurred == false );

  TEST_END();
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK        1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS        9

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
