/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <signal.h>

volatile int Signal_occurred;
volatile int Signal_count;
void Signal_handler( int signo );

void Signal_handler(
  int signo
)
{
  Signal_count++;
  printf(
    "Signal: %d caught by 0x%x (%d)\n",
    (int) signo,
    (unsigned int) pthread_self(),
    Signal_count
  );
  Signal_occurred = 1;
}

void *POSIX_Init(
  void *argument
)
{
  int               status;
  struct timespec   tv;
  struct timespec   tr;
  struct sigaction  act;
  sigset_t          mask;
  sigset_t          pending_set;

  puts( "\n\n*** POSIX TEST 2 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* install a signal handler */

  status = sigemptyset( &act.sa_mask );
  rtems_test_assert( !status );

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;

  sigaction( SIGUSR1, &act, NULL );

  /* simple signal to self */

  Signal_count = 0;
  Signal_occurred = 0;

  status = pthread_kill( Init_id, SIGUSR1 );
  rtems_test_assert( !status );

  Signal_occurred = 0;

  /* now block the signal, send it, see if it is pending, and unblock it */

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "Init: Block SIGUSR1\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !status );

  status = sigpending( &pending_set );
  rtems_test_assert( !status );
  printf( "Init: Signals pending 0x%08x\n", (unsigned int) pending_set );


  printf( "Init: send SIGUSR1 to self\n" );
  status = pthread_kill( Init_id, SIGUSR1 );
  rtems_test_assert( !status );

  status = sigpending( &pending_set );
  rtems_test_assert( !status );
  printf( "Init: Signals pending 0x%08x\n", (unsigned int) pending_set );

  printf( "Init: Unblock SIGUSR1\n" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !status );

  /* create a thread */

  status = pthread_create( &Task_id, NULL, Task_1_through_3, NULL );
  rtems_test_assert( !status );

  /*
   *  Loop for 5 seconds seeing how many signals we catch
   */

  tr.tv_sec = 5;
  tr.tv_nsec = 0;

  do {
    tv = tr;

    Signal_occurred = 0;

    status = nanosleep ( &tv, &tr );

    if ( status == -1 ) {
      rtems_test_assert( errno == EINTR );
      rtems_test_assert( tr.tv_nsec || tr.tv_sec );
    } else if ( !status ) {
      rtems_test_assert( !tr.tv_nsec && !tr.tv_sec );
    }

    printf(
      "Init: signal was %sprocessed with %d:%d time remaining\n",
      (Signal_occurred) ? "" : "not ",
      (int) tr.tv_sec,
      (int) tr.tv_nsec
   );

  } while ( tr.tv_sec || tr.tv_nsec );

  /* exit this thread */

  puts( "*** END OF POSIX TEST 2 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
