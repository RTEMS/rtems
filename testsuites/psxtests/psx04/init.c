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
#include <errno.h>

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
  int               status;
  struct sigaction  act;
  sigset_t          mask;
  sigset_t          pending_set;
  sigset_t          oset;
  struct timespec   timeout;
  siginfo_t         info;

  puts( "\n\n*** POSIX TEST 4 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* generate some easy error cases */

  status = sigwait( NULL, NULL );
  if ( status != EINVAL )
    printf( "status = %d (%s)\n", status, strerror(status) );
  rtems_test_assert( status == EINVAL );
  puts( "Init: sigwait - EINVAL (NULL set)" );

  status = sigtimedwait( NULL, NULL, NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigwait - EINVAL (NULL set)" );

/* install a signal handler for SIGUSR1 */

  status = sigemptyset( &act.sa_mask );
  rtems_test_assert( !status );
  printf( "Init: sigemptyset -  set= 0x%08x\n", (unsigned int) act.sa_mask );

  /* test sigfillset following the above sigemptyset */

  status = sigfillset( &act.sa_mask );
  rtems_test_assert( !status );
  printf( "Init: sigfillset -  set= 0x%08x\n", (unsigned int) act.sa_mask );

  /* test sigdelset */

  status = sigdelset( &act.sa_mask, SIGUSR1 );
  rtems_test_assert( !status );
  printf( "Init: sigdelset - delete SIGUSR1 set= 0x%08x\n",
      (unsigned int) act.sa_mask );

  /* test sigismember - FALSE */

  status = sigismember( &act.sa_mask, SIGUSR1 );
  rtems_test_assert( !status );
  puts( "Init: sigismember - FALSE since SIGUSR1 is not a member" );

  /* test sigismember - TRUE */

  status = sigismember( &act.sa_mask, SIGUSR2 );
  rtems_test_assert( status );
  puts( "Init: sigismember - TRUE since SIGUSR2 is a member" );

  /* return the set to empty */

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;

  sigaction( SIGUSR1, &act, NULL );

  /* simple signal to process */

  Signal_count = 0;
  Signal_occurred = 0;

  puts( "Init: send SIGUSR1 to process" );
  status = kill( getpid(), SIGUSR1 );
  rtems_test_assert( !status );

/* end of install a signal handler for SIGUSR1 */

  Signal_occurred = 0;

  /* now block the signal, send it, see if it is pending, and unblock it */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  puts( "Init: Block SIGUSR1" );
  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;

  sigaction( SIGUSR1, &act, NULL );

  /* simple signal to process */

  Signal_count = 0;
  Signal_occurred = 0;

  puts( "Init: send SIGUSR1 to process" );
  status = kill( getpid(), SIGUSR1 );
  rtems_test_assert( !status );

  Signal_occurred = 0;

  /* now block the signal, send it, see if it is pending, and unblock it */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  puts( "Init: Block SIGUSR1" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !status );

  status = sigpending( &pending_set );
  rtems_test_assert( !status );
  printf( "Init: Signals pending 0x%08x\n", (unsigned int) pending_set );

  puts( "Init: send SIGUSR1 to process" );
  status = kill( getpid(), SIGUSR1 );
  rtems_test_assert( !status );

  status = sigpending( &pending_set );
  rtems_test_assert( !status );
  printf( "Init: Signals pending 0x%08x\n", (unsigned int) pending_set );

  puts( "Init: Unblock SIGUSR1" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !status );

  /* now let another task get interrupted by a signal */

  empty_line();

  puts( "Init: create a thread interested in SIGUSR1" );
  status = pthread_create( &Task1_id, NULL, Task_1, NULL );
  rtems_test_assert( !status );

  puts( "Init: Block SIGUSR1" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !status );

  status = sigpending( &pending_set );
  rtems_test_assert( !status );
  printf( "Init: Signals pending 0x%08x\n", (unsigned int) pending_set );

  puts( "Init: sleep so the other task can block" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );

     /* switch to task 1 */

  puts( "Init: send SIGUSR1 to process" );
  status = kill( getpid(), SIGUSR1 );
  rtems_test_assert( !status );

  status = sigpending( &pending_set );
  rtems_test_assert( !status );
  printf( "Init: Signals pending 0x%08x\n", (unsigned int) pending_set );

  puts( "Init: sleep so the other task can catch signal" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );

     /* switch to task 1 */

  /* test alarm */

  empty_line();

  /* install a signal handler for SIGALRM and unblock it */

  status = sigemptyset( &act.sa_mask );
  rtems_test_assert( !status );

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;

  sigaction( SIGALRM, &act, NULL );

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGALRM );
  rtems_test_assert( !status );

  puts( "Init: Unblock SIGALRM" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !status );

  /* schedule the alarm */

  puts( "Init: Firing alarm in 5 seconds" );
  remaining = alarm( 5 );
  printf( "Init: %d seconds left on previous alarm\n", status );
  rtems_test_assert( !status );

  puts( "Init: Firing alarm in 2 seconds" );
  remaining = alarm( 2 );
  printf( "Init: %d seconds left on previous alarm\n", remaining );
  rtems_test_assert( remaining == 5 );

  puts( "Init: Wait 4 seconds for alarm" );
  remaining = sleep( 4 );
  printf( "Init: %d seconds left in sleep\n", remaining );
  rtems_test_assert( remaining == 2 );

  /* test SIG_SETMASK case and returning oset of pthread_sigmask */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR2 );
  rtems_test_assert( !status );

  puts( "Init: Block SIGUSR1 and SIGUSR2 only" );
  status = pthread_sigmask( SIG_SETMASK, &mask, &oset );
  printf( "Init: Previous blocked set was 0x%08x\n", (unsigned int) oset );
  rtems_test_assert( !status );

  /* test inquiry about current blocked set with pthread_sigmask */

  status = pthread_sigmask( 0, NULL, &oset );
  printf( "Init: Current blocked set is 0x%08x\n", (unsigned int) oset );
  rtems_test_assert( !status );

  /* return blocked mask to no signals blocked */

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  puts( "Init: Unblock all signals" );
  status = pthread_sigmask( SIG_SETMASK, &mask, &oset );
  printf( "Init: Previous blocked set was 0x%08x\n", (unsigned int) oset );
  rtems_test_assert( !status );

  /* test sigsuspend */

  empty_line();

  puts( "Init: create a thread to send Init SIGUSR1" );
  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  rtems_test_assert( !status );

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  puts( "Init: sigsuspend for any signal" );
  status = sigsuspend( &mask );
  rtems_test_assert( status );
  printf( "Init: awakended from sigsuspend status=%08d \n", status );

  /* test a SIGINFO case, these are signals sent to a process only */

  empty_line();

  puts( "Init: create a thread to sent Process SIGUSR1 with SA_SIGINFO" );
  status = pthread_create( &Task3_id, NULL, Task_3, NULL );
  rtems_test_assert( !status );

  /* set action on SIGUSR1 to an info case */
  act.sa_handler   = Signal_handler;
  act.sa_flags     = SA_SIGINFO;
  act.sa_sigaction = Signal_info_handler;

  sigaction( SIGUSR1, &act, NULL );

  puts( "Init: sleep so the Task_3 can sigqueue SIGUSR1" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );

     /* switch to task 1 */

  puts( "Init: sigqueue occurred" );

  /* Send SIGUSR1, Task_3 has issued a sigwaitinfo */

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  puts( "Init: Block SIGUSR1" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !status );

  puts( "Init: send SIGUSR1 to process" );
  status = kill( getpid(), SIGUSR1 );
  rtems_test_assert( !status );

  puts( "Init: sleep so the Task_3 can receive SIGUSR1" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );

  /* Send SIGUSR1, Task_3 has issued a sigwait */

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  puts( "Init: Block SIGUSR1" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !status );

  puts( "Init: send SIGUSR1 to process" );
  status = kill( getpid(), SIGUSR1 );
  rtems_test_assert( !status );

  puts( "Init: sleep so the Task_3 can receive SIGUSR1" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );

  /* Send SIGUSR1, Task_3 has issued a sigwaitinfo */

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR2 );
  rtems_test_assert( !status );

  puts( "Init: Block SIGUSR2" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  rtems_test_assert( !status );

  puts( "Init: send SIGUSR2 to process" );
  status = kill( getpid(), SIGUSR2 );
  rtems_test_assert( !status );

  puts( "Init: sleep so the Task_3 can receive SIGUSR2" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );

  /* Suspend for signal that has already be sent */

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  puts( "Init: sigsuspend for any signal" );
  status = sigsuspend( &mask );
  rtems_test_assert( status );
  printf( "Init: awakended from sigsuspend status=%d \n", status );

  /* generate error cases for psignal */

  empty_line();

  status = sigemptyset( NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigemptyset - EINVAL (set invalid)" );

  status = sigfillset( NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigfillset - EINVAL (set invalid)" );

  status = sigaddset( NULL, SIGUSR1 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigaddset - EINVAL (set invalid)" );

  status = sigaddset( &mask, 0 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigaddset - EINVAL (signal = 0)" );

  status = sigaddset( &mask, 999 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigaddset - EINVAL (set invalid)" );

  status = sigdelset( NULL, SIGUSR1 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigdelset - EINVAL (set invalid)" );

  status = sigdelset( &mask, 0 );
  rtems_test_assert( !status );
  puts( "Init: sigdelset - SUCCESSFUL (signal = 0)" );

  status = sigdelset( &mask, 999 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigdelset - EINVAL (set invalid)" );

  status = sigismember( NULL, SIGUSR1 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigismember - EINVAL (set invalid)" );

  status = sigismember( &mask, 0 );
  rtems_test_assert( !status );
  puts( "Init: sigismember - SUCCESSFUL (signal = 0)" );

  status = sigismember( &mask, 999 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigismember - EINVAL (signal invalid)" );

  status = sigaction( 0, &act, 0 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigaction - EINVAL (signal = 0)" );

  status = sigaction( 999, &act, NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigaction - EINVAL (signal invalid)" );

  status = sigaction( SIGKILL, &act, NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigaction - EINVAL (SIGKILL)" );

  status = pthread_sigmask( SIG_BLOCK, NULL, NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: pthread_sigmask - EINVAL (set and oset invalid)" );

  status = pthread_sigmask( 999, &pending_set, NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: pthread_sigmask - EINVAL (how invalid)" );

  status = sigpending( NULL );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: sigpending - EINVAL (set invalid)" );

  timeout.tv_nsec = -1;
  status = sigtimedwait( &mask, &info, &timeout );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: pthread_sigmask - EINVAL (timout->nsec invalid < 0)" );

  timeout.tv_nsec = 0x7fffffff;
  status = sigtimedwait( &mask, &info, &timeout );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: pthread_sigmask - EINVAL (timout->nsec invalid to large)" );

  status = pthread_kill( Init_id, 999 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: pthread_kill - EINVAL (sig invalid)" );

  status = pthread_kill( Init_id, 0 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: pthread_kill - EINVAL (signal = 0)" );

  act.sa_handler = SIG_IGN;
  act.sa_flags = 0;
  sigaction( SIGUSR2, &act, NULL );
  status = pthread_kill( Init_id, SIGUSR2 );
  rtems_test_assert( !status );
  puts( "Init: pthread_kill - SUCCESSFUL (signal = SIG_IGN)" );

  status = kill( INT_MAX, SIGUSR1 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == ESRCH );
  puts( "Init: kill - ESRCH (pid invalid)" );

  status = kill( getpid(), 0 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: kill - EINVAL (signal = 0)" );

  status = kill( getpid(), 999 );
  if ( status != -1 )
    printf( "status = %d\n", status );
  rtems_test_assert( errno == EINVAL );
  puts( "Init: kill - EINVAL (sig invalid)" );

  /* exit this thread */

  puts( "*** END OF POSIX TEST 4 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
