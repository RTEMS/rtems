/**
 *  @file
 *
 *  Based upon user code supplied in conjunction with PR1759
 */

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

#include "tmacros.h"

#include <stdio.h>
#include <rtems.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

int       Caught_signo = -1;
siginfo_t Caught_siginfo = { -1, -1, };

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument arg);
void handler(int signo);
void handler_info(int signo, siginfo_t *info, void *context);
rtems_task test_task(rtems_task_argument arg);

void handler(int signo)
{
  Caught_signo = signo;
}

void handler_info(int signo, siginfo_t *info, void *context)
{
  Caught_signo = signo;
  Caught_siginfo = *info;
}

rtems_task test_task(rtems_task_argument arg)
{
  int sc;
  struct sigaction new_action;
  sigset_t mask;

  printf("test_task starting...\n");

  sc = sigemptyset (&new_action.sa_mask);
  rtems_test_assert( sc == 0 );

  sc = sigfillset  (&new_action.sa_mask);
  rtems_test_assert( sc == 0 );

  sc = sigdelset   (&new_action.sa_mask, SIGUSR1);
  rtems_test_assert( sc == 0 );

  new_action.sa_handler = handler;
  new_action.sa_flags = SA_SIGINFO;
  new_action.sa_sigaction = handler_info;

  sc = sigaction(SIGUSR1,&new_action,NULL);
  rtems_test_assert( sc == 0 );

  sc = sigemptyset(&mask);
  rtems_test_assert( sc == 0 );

  sc = sigaddset(&mask, SIGUSR1);
  rtems_test_assert( sc == 0 );

  sc = pthread_sigmask( SIG_UNBLOCK, &mask, NULL);
  rtems_test_assert( sc == 0 );

  printf("test_task waiting for signal...\n");

  while(1) {
    sleep(1);
    if ( Caught_siginfo.si_signo != -1 ) {
      printf( "Signal_info: %d si_signo= %d si_code= %d value= %d\n",
        Caught_signo,
        Caught_siginfo.si_signo,
        Caught_siginfo.si_code,
        Caught_siginfo.si_value.sival_int
      );
      break;
    }
    if ( Caught_signo != -1 ) {
      printf( "Signal: %d caught\n", Caught_signo );
      break;
    }
  }
  puts( "test_task exiting thread" );
  pthread_exit( 0 );
}

rtems_task Init(rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_id          task_id;
  int               status;
  void             *retval;

  puts( "*** START OF CLASSIC API TEST OF POSIX 01 ***" );

  sc = rtems_task_create(
    rtems_build_name('T','E','S','T'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_start( task_id,  test_task, 0 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );


  puts( "Init - pthread_equal on Classic Ids" );
  status = pthread_equal( task_id, task_id );
  rtems_test_assert( status != 0 );
  
  puts( "Init - pthread_cancel on Classic Task" );
  status = pthread_cancel( task_id );
  rtems_test_assert( status == 0 );
  
  status = pthread_detach( task_id );
  rtems_test_assert( status == 0 );

  status = pthread_join( task_id, &retval );
  if ( status != EINVAL ) printf( "JOIN %s\n", strerror( status ) );
  rtems_test_assert( status == EINVAL );

  status = pthread_kill( task_id, SIGUSR1 );
  rtems_test_assert( status == 0 );

  puts( "*** END OF CLASSIC API TEST OF POSIX 01 ***" );
  exit(0);
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INIT_TASK_INITIAL_MODES \
  (RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
