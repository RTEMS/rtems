/*  Task_3
 *
 *  This routine serves as a test task.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include <signal.h>

void *Task_3(
  void *argument
)
{
  unsigned int  remaining;
  int           status;
  int           sig;
  volatile union sigval  value; /* should be removed once the H8300 target is fixed */
  sigset_t      mask;
  siginfo_t     info;

  value.sival_int = SIGUSR1;

  printf( "Task_3: sigqueue SIGUSR1 with value= %d\n", value.sival_int );
  status = sigqueue( getpid(), SIGUSR1, value );
  rtems_test_assert( !status );

     /* catch signal with sigwaitinfo */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "Task_3: sigwaitinfo SIGUSR1 with value= %d\n", value.sival_int );
  status = sigwaitinfo( &mask, &info );

     /* switch to Init */

  rtems_test_assert( !(status==-1) );
  printf(
    "Task_3: si_signo= %d si_code= %d value= %d\n",
    info.si_signo,
    info.si_code,
    info.si_value.sival_int
  );

     /* catch signal with sigwait */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "Task_3: sigwait SIGUSR1\n" );
  status = sigwait( &mask, &sig );

     /* switch to Init */

  rtems_test_assert( !status );
  printf( "Task_3: signo= %d\n", sig );

     /* catch signal with pause */

  empty_line();

  status = sigemptyset( &mask );
  rtems_test_assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "Task_3: pause\n" );
  status = pause( );

     /* switch to Init */

  rtems_test_assert( !(status==-1) );
  printf( "Task_3: pause= %d\n", status );


     /* send signal to Init task before it has pended for a signal */

  empty_line();

  printf( "Task_3: sending SIGUSR2\n" );
  status = pthread_kill( Init_id, SIGUSR2 );
  rtems_test_assert( !status );

  printf( "Task_3: sleep so the Init task can reguest a signal\n" );
  remaining = sleep( 1 );
  rtems_test_assert( !status );
  rtems_test_assert( remaining == 0 );

     /* end of task 3 */
  printf( "Task_3: exit\n" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}
