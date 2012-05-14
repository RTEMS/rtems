/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pmacros.h>

#include <signal.h>
#include <errno.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

volatile int Signal_occurred;
volatile int Signal_count;
void Signal_handler( int signo );
void Signal_info_handler(
  int        signo,
  siginfo_t *info,
  void      *context
);

void Signal_handler(
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
  Signal_occurred = 1;
}

void Signal_info_handler(
  int        signo,
  siginfo_t *info,
  void      *context
)
{
  Signal_count++;
  printf(
    "Signal_info: %d caught by 0x%" PRIxpthread_t " (%d) si_signo= %d si_code= %d value= %d\n",
    signo,
    pthread_self(),
    Signal_count,
    info->si_signo,
    info->si_code,
    info->si_value.sival_int
  );
  Signal_occurred = 1;
}

void *POSIX_Init(
  void *argument
)
{
  unsigned int      remaining;
  int               sc;
  struct sigaction  act;
  sigset_t          mask;

  puts( "\n\n*** POSIX ALARM TEST 01 ***" );

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

  puts( "Init: Cancel alarm" );
  remaining = alarm( 0 );
  printf( "Init: %d seconds left on previous alarm\n", remaining );
  rtems_test_assert( remaining == 0 );

  puts( "*** END OF POSIX ALARM TEST 01***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS        1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
        (RTEMS_MINIMUM_STACK_SIZE * 4)

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

