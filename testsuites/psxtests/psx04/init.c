/* 
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <signal.h>

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

void Signal_info_handler(
  int        signo,
  siginfo_t *info,
  void      *context
)
{
  Signal_count++;
  printf(
    "Signal_info: %d caught by 0x%x (%d) si_signo= %d si_code= %d value= %d\n",
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
  int               status;
  struct sigaction  act;
  sigset_t          mask;
  sigset_t          pending_set;
  sigset_t          oset;

  puts( "\n\n*** POSIX TEST 4 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );

/* install a signal handler for SIGUSR1 */

  status = sigemptyset( &act.sa_mask );
  assert( !status );
  printf( "Init: sigemptyset -  set= 0x%08x\n", act.sa_mask );

  /* test sigfillset following the above sigemptyset */

  status = sigfillset( &act.sa_mask );
  assert( !status );
  printf( "Init: sigfillset -  set= 0x%08x\n", act.sa_mask );

  /* test sigdelset */

  status = sigdelset( &act.sa_mask, SIGUSR1 );
  assert( !status );
  printf( "Init: sigdelset - delete SIGUSR1 set= 0x%08x\n", act.sa_mask );

  /* test sigismember - FALSE */

  status = sigismember( &act.sa_mask, SIGUSR1 );
  assert( !status );
  puts( "Init: sigismember - FALSE since SIGUSR1 is not a member" );

  /* test sigismember - TRUE */

  status = sigismember( &act.sa_mask, SIGUSR2 );
  assert( status );
  puts( "Init: sigismember - TRUE since SIGUSR2 is a member" );

  /* return the set to empty */

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
 
  sigaction( SIGUSR1, &act, NULL );

  /* simple signal to process */

  Signal_count = 0;
  Signal_occurred = 0;

  printf( "Init: send SIGUSR1 to process\n" );
  status = kill( getpid(), SIGUSR1 );
  assert( !status );

/* end of install a signal handler for SIGUSR1 */

  Signal_occurred = 0;

  /* now block the signal, send it, see if it is pending, and unblock it */

  empty_line();

  status = sigemptyset( &mask );
  assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  assert( !status );

  printf( "Init: Block SIGUSR1\n" );
  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
 
  sigaction( SIGUSR1, &act, NULL );

  /* simple signal to process */

  Signal_count = 0;
  Signal_occurred = 0;

  printf( "Init: send SIGUSR1 to process\n" );
  status = kill( getpid(), SIGUSR1 );
  assert( !status );

  Signal_occurred = 0;

  /* now block the signal, send it, see if it is pending, and unblock it */

  empty_line();

  status = sigemptyset( &mask );
  assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  assert( !status );

  printf( "Init: Block SIGUSR1\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  assert( !status );

  status = sigpending( &pending_set );
  assert( !status );
  printf( "Init: Signals pending 0x%08x\n", pending_set );
  
  printf( "Init: send SIGUSR1 to process\n" );
  status = kill( getpid(), SIGUSR1 );
  assert( !status );

  status = sigpending( &pending_set );
  assert( !status );
  printf( "Init: Signals pending 0x%08x\n", pending_set );
  
  printf( "Init: Unblock SIGUSR1\n" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  assert( !status );

  /* now let another task get interrupted by a signal */

  empty_line();

  printf( "Init: create a thread interested in SIGUSR1\n" );
  status = pthread_create( &Task1_id, NULL, Task_1, NULL );
  assert( !status );

  printf( "Init: Block SIGUSR1\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  assert( !status );
 
  status = sigpending( &pending_set );
  assert( !status );
  printf( "Init: Signals pending 0x%08x\n", pending_set );

  printf( "Init: sleep so the other task can block\n" ); 
  status = sleep( 1 );
  assert( !status );

     /* switch to task 1 */

  printf( "Init: send SIGUSR1 to process\n" );
  status = kill( getpid(), SIGUSR1 );
  assert( !status );
 
  status = sigpending( &pending_set );
  assert( !status );
  printf( "Init: Signals pending 0x%08x\n", pending_set );

  printf( "Init: sleep so the other task can catch signal\n" ); 
  status = sleep( 1 );
  assert( !status );

     /* switch to task 1 */

  /* test alarm */

  empty_line();

  /* install a signal handler for SIGALRM and unblock it */
 
  status = sigemptyset( &act.sa_mask );
  assert( !status );
 
  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
 
  sigaction( SIGALRM, &act, NULL );
 
  status = sigemptyset( &mask );
  assert( !status );
 
  status = sigaddset( &mask, SIGALRM );
  assert( !status );
 
  printf( "Init: Unblock SIGALRM\n" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  assert( !status );

  /* schedule the alarm */
 
  printf( "Init: Firing alarm in 5 seconds\n" );
  status = alarm( 5 );
  printf( "Init: %d seconds left on previous alarm\n", status );
  assert( !status );

  printf( "Init: Firing alarm in 2 seconds\n" );
  status = alarm( 2 );
  printf( "Init: %d seconds left on previous alarm\n", status );
  assert( status );

  printf( "Init: Wait 4 seconds for alarm\n" );
  status = sleep( 4 );
  printf( "Init: %d seconds left in sleep\n", status );
  assert( status );

  /* test SIG_SETMASK case and returning oset of pthread_sigmask */

  empty_line();

  status = sigemptyset( &mask );
  assert( !status );
 
  status = sigaddset( &mask, SIGUSR1 );
  assert( !status );

  status = sigaddset( &mask, SIGUSR2 );
  assert( !status );

  printf( "Init: Block SIGUSR1 and SIGUSR2 only\n" );
  status = pthread_sigmask( SIG_SETMASK, &mask, &oset );
  printf( "Init: Previous blocked set was 0x%08x\n", oset );
  assert( !status );

  /* test inquiry about current blocked set with pthread_sigmask */
  
  status = pthread_sigmask( NULL, NULL, &oset );
  printf( "Init: Current blocked set is 0x%08x\n", oset );
  assert( !status );

  /* return blocked mask to no signals blocked */

  status = sigemptyset( &mask );
  assert( !status );
 
  printf( "Init: Unblock all signals\n" );
  status = pthread_sigmask( SIG_SETMASK, &mask, &oset );
  printf( "Init: Previous blocked set was 0x%08x\n", oset );
  assert( !status );

  /* test sigsuspend */

  empty_line();

  printf( "Init: create a thread to send Init SIGUSR1\n" );
  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  assert( !status );

  status = sigemptyset( &mask );
  assert( !status );
 
  printf( "Init: sigsuspend for any signal\n" );
  status = sigsuspend( &mask );
  assert( status );
  printf( "Init: awakended from sigsuspend status=%08d \n", status );

  /* test a SIGINFO case, these are signals sent to a process only */

  empty_line();

  printf( "Init: create a thread to sent Process SIGUSR1 with SA_SIGINFO\n" );
  status = pthread_create( &Task3_id, NULL, Task_3, NULL );
  assert( !status );

  /* set action on SIGUSR1 to an info case */
  act.sa_handler   = Signal_handler;
  act.sa_flags     = SA_SIGINFO;
  act.sa_sigaction = Signal_info_handler;
 
  sigaction( SIGUSR1, &act, NULL );

  printf( "Init: sleep so the Task_3 can sigqueue SIGUSR1\n" ); 
  status = sleep( 1 );
  assert( !status );

     /* switch to task 1 */

  puts( "Init: sigqueue occurred" );

  /* Send SIGUSR1, Task_3 has issued a sigwaitinfo */

  status = sigemptyset( &mask );
  assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  assert( !status );

  printf( "Init: Block SIGUSR1\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  assert( !status );

  printf( "Init: send SIGUSR1 to process\n" );
  status = kill( getpid(), SIGUSR1 );
  assert( !status );

  printf( "Init: sleep so the Task_3 can receive SIGUSR1\n" ); 
  status = sleep( 1 );
  assert( !status );

  /* Send SIGUSR1, Task_3 has issued a sigwait */
 
  status = sigemptyset( &mask );
  assert( !status );
 
  status = sigaddset( &mask, SIGUSR1 );
  assert( !status );
 
  printf( "Init: Block SIGUSR1\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  assert( !status );
 
  printf( "Init: send SIGUSR1 to process\n" );
  status = kill( getpid(), SIGUSR1 );
  assert( !status );
 
  printf( "Init: sleep so the Task_3 can receive SIGUSR1\n" );
  status = sleep( 1 );
  assert( !status );

  /* Send SIGUSR1, Task_3 has issued a sigwaitinfo */
 
  status = sigemptyset( &mask );
  assert( !status );
 
  status = sigaddset( &mask, SIGUSR2 );
  assert( !status );
 
  printf( "Init: Block SIGUSR2\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  assert( !status );
 
  printf( "Init: send SIGUSR2 to process\n" );
  status = kill( getpid(), SIGUSR2 );
  assert( !status );
 
  printf( "Init: sleep so the Task_3 can receive SIGUSR2\n" );
  status = sleep( 1 );
  assert( !status );

  /* Suspend for signal that has already be sent */

  printf( "Init: sigsuspend for any signal\n" );
  status = sigsuspend( &mask );
  assert( status );
  printf( "Init: awakended from sigsuspend status=%d \n", status );

  /* exit this thread */

  puts( "*** END OF POSIX TEST 4 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
