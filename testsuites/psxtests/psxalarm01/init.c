/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

