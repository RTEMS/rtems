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

#define CONFIGURE_INIT
#include "system.h"
#include <signal.h>
#include <unistd.h>
#include <errno.h>

/* forward declarations to avoid warnings */
void Signal_handler(int signo);
rtems_timer_service_routine Signal_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
);

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
extern void _POSIX_signals_Abnormal_termination_handler( int signo );

volatile int Signal_occurred;
volatile int Signal_count;

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

rtems_timer_service_routine Signal_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  int               status;

  status = kill( getpid(), SIGUSR1 );
  rtems_test_assert( status == 0 );
}


void *POSIX_Init(
  void *argument
)
{
  int               status;
  useconds_t        result;
  struct sigaction  act;
  sigset_t          mask;

  puts( "\n\n*** POSIX TEST UALARM ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  Signal_occurred = 0;
  Signal_count = 0;

  /* Validate ualarm is ignored if signal not caught */
  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
  sigaction( SIGALRM, &act, NULL );
  puts( "Init: ualarm in 1 us" );
  sleep(3);
  result = ualarm(1,0);
  rtems_test_assert( result == 0 );
  
  status = sleep(10);
  rtems_test_assert( status == 0 );

  /* unblock Signal and see if it happened */
  status = sigemptyset( &mask );
  rtems_test_assert( !status );
  status = sigaddset( &mask, SIGALRM );
  rtems_test_assert( !status );
  puts( "Init: Unblock SIGALRM" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !status );
  status = sleep(10);

  /* stop ularm */
  puts( "Init: clear ualarm with 0,0" );
  result = ualarm(0,0);
  status = sleep(10);

  puts( "*** END OF POSIX TEST UALARM ***" );
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}
