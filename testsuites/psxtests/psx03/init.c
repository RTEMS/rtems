/* 
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <signal.h>
#include <errno.h>

volatile int Signal_occurred;
volatile int Signal_count;

void Signal_handler(
  int signo
)
{
  Signal_count++;
  printf(
    "Signal: %d caught by 0x%x (%d)\n",
    signo,
    pthread_self(),
    Signal_count
  );
  Signal_occurred = 1;
}

void *POSIX_Init(
  void *argument
)
{
  int               status;
  struct timespec   timeout;
  struct sigaction  act;
  sigset_t          mask;
  sigset_t          waitset;
  int               signo;
  siginfo_t         siginfo;

  puts( "\n\n*** POSIX TEST 3 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );

  /* install a signal handler */

  status = sigemptyset( &act.sa_mask );
  assert( !status );

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
 
  sigaction( SIGUSR1, &act, NULL );

  /* initialize signal handler variables */

  Signal_count = 0;
  Signal_occurred = 0;

  /*
   *  wait on SIGUSR1 for 3 seconds, will timeout 
   */

  /* initialize the signal set we will wait for to SIGUSR1 */

  status = sigemptyset( &waitset );
  assert( !status );

  status = sigaddset( &waitset, SIGUSR1 );
  assert( !status );

  timeout.tv_sec = 3;
  timeout.tv_nsec = 0;

  puts( "Init: waiting on any signal for 3 seconds." );
  signo = sigtimedwait( &waitset, &siginfo, &timeout );
  assert( signo == -1 );

  if ( errno == EAGAIN ) 
    puts( "Init: correctly timed out waiting for SIGUSR1." );
  else
    printf( "sigtimedwait returned wrong errno - %d\n", errno );

  Signal_occurred = 0;

  /*
   *  wait on SIGUSR1 for 3 seconds, will timeout because Task_1 sends SIGUSR2
   */

  empty_line();

  /* initialize a mask to block SIGUSR2 */

  status = sigemptyset( &mask );
  assert( !status );

  status = sigaddset( &mask, SIGUSR2 );
  assert( !status );

  printf( "Init: Block SIGUSR2\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  assert( !status );

  /* create a thread */

  status = pthread_create( &Task_id, NULL, Task_1, NULL );
  assert( !status );

  /* signal handler is still installed, waitset is still set for SIGUSR1 */
   
  timeout.tv_sec = 3;
  timeout.tv_nsec = 0;
 
  puts( "Init: waiting on any signal for 3 seconds." );
  signo = sigtimedwait( &waitset, &siginfo, &timeout );

     /* switch to Task 1 */

  if ( errno == EAGAIN )
    puts( "Init: correctly timed out waiting for SIGUSR1." );
  else
    printf( "sigtimedwait returned wrong errno - %d\n", errno );
  assert( signo == -1 );
 
  /*
   *  wait on SIGUSR1 for 3 seconds, Task_2 will send it to us
   */

  empty_line();

  /* create a thread */

  status = pthread_create( &Task_id, NULL, Task_2, NULL );
  assert( !status );

  /* signal handler is still installed, waitset is still set for SIGUSR1 */
 
  /* wait on SIGUSR1 for 3 seconds, will receive SIGUSR1 from Task_2 */
 
  timeout.tv_sec = 3;
  timeout.tv_nsec = 0;
 
  /* just so we can check that these were altered */

  siginfo.si_code = -1;
  siginfo.si_signo = -1;
  siginfo.si_value.sival_int = -1;

  puts( "Init: waiting on any signal for 3 seconds." );
  signo = sigtimedwait( &waitset, &siginfo, &timeout );
  printf( "Init: received (%d) SIGUSR1=%d\n", siginfo.si_signo, SIGUSR1 );
  assert( signo == SIGUSR1 );
  assert( siginfo.si_signo == SIGUSR1 );
  assert( siginfo.si_code == SI_USER );
  assert( siginfo.si_value.sival_int != -1 );   /* rtems does always set this */
 
  /* try out a process signal */

  empty_line();
  puts( "Init: kill with SIGUSR2." );
  status = kill( getpid(), SIGUSR2 );
  assert( !status );

  siginfo.si_code = -1;
  siginfo.si_signo = -1;
  siginfo.si_value.sival_int = -1;

  status = sigemptyset( &waitset );
  assert( !status );

  status = sigaddset( &waitset, SIGUSR1 );
  assert( !status );

  status = sigaddset( &waitset, SIGUSR2 );
  assert( !status );

  puts( "Init: waiting on any signal for 3 seconds." );
  signo = sigtimedwait( &waitset, &siginfo, &timeout );
  printf( "Init: received (%d) SIGUSR2=%d\n", siginfo.si_signo, SIGUSR2 );
  assert( signo == SIGUSR2 );
  assert( siginfo.si_signo == SIGUSR2 );
  assert( siginfo.si_code == SI_USER );
  assert( siginfo.si_value.sival_int != -1 );   /* rtems does always set this */

  /* exit this thread */

  puts( "*** END OF POSIX TEST 3 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
