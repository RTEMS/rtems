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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <rtems.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include <tmacros.h>

const char rtems_test_name[] = "PSXCLASSIC 1";

static int       Caught_signo = -1;
static siginfo_t Caught_siginfo = { -1, -1, };

static void handler(int signo)
{
  Caught_signo = signo;
}

static void handler_info(int signo, siginfo_t *info, void *context)
{
  Caught_signo = signo;
  Caught_siginfo = *info;
}

static rtems_task test_task(rtems_task_argument arg)
{
  int sc;
  struct sigaction new_action;
  sigset_t mask;
  void *addr;
  size_t size;
  int value;
  struct sched_param param;
  cpu_set_t set;
  pthread_attr_t attr;

  printf("test_task starting...\n");

  value = -1;
  memset( &param, -1, sizeof( param ) );
  sc = pthread_getschedparam( pthread_self(), &value, &param );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( value == SCHED_FIFO );
  rtems_test_assert(
    param.sched_priority == sched_get_priority_max( SCHED_FIFO )
  );

  sc = pthread_setschedparam( pthread_self(), value, &param );
  rtems_test_assert( sc == 0 );

  sc = pthread_getattr_np( pthread_self(), &attr );
  rtems_test_assert( sc == 0 );

  addr = NULL;
  size = 0;
  sc = pthread_attr_getstack( &attr, &addr, &size );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( addr != NULL );
  rtems_test_assert( size == RTEMS_MINIMUM_STACK_SIZE );

  value = -1;
  sc = pthread_attr_getscope( &attr, &value );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( value == PTHREAD_SCOPE_PROCESS );

  value = -1;
  sc = pthread_attr_getinheritsched( &attr, &value );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( value == PTHREAD_EXPLICIT_SCHED );

  value = -1;
  sc = pthread_attr_getschedpolicy( &attr, &value );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( value == SCHED_FIFO );

  memset( &param, -1, sizeof( param ) );
  sc = pthread_attr_getschedparam( &attr, &param );
  rtems_test_assert( sc == 0 );
  rtems_test_assert(
    param.sched_priority == sched_get_priority_max( SCHED_FIFO )
  );

  size = 1;
  sc = pthread_attr_getguardsize( &attr, &size );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( size == 0 );

  value = -1;
  sc = pthread_attr_getdetachstate( &attr, &value );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( value == PTHREAD_CREATE_JOINABLE );

  CPU_ZERO( &set );
  sc = pthread_attr_getaffinity_np( &attr, sizeof( set ), &set );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( CPU_ISSET( 0, &set ) );
  rtems_test_assert( CPU_COUNT( &set ) == 1 );

  sc = pthread_attr_destroy( &attr );
  rtems_test_assert( sc == 0 );

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
  pthread_exit( (void *) 123 );
}


static rtems_id create_task( void )
{
  rtems_status_code sc;
  rtems_id          task_id;

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

  return task_id;
}

static rtems_task Init( rtems_task_argument arg )
{
  rtems_id  task_id;
  int       status;
  void     *retval;

  TEST_BEGIN();

  task_id = create_task();

  puts( "Init - pthread_equal on Classic Ids" );
  status = pthread_equal( task_id, task_id );
  rtems_test_assert( status != 0 );
  
  puts( "Init - pthread_cancel on Classic Task" );
  status = pthread_cancel( task_id );
  rtems_test_assert( status == 0 );
  
  status = pthread_detach( task_id );
  rtems_test_assert( status == 0 );

  retval = (void *) 456;
  status = pthread_join( task_id, &retval );
  rtems_test_assert( status == ESRCH );
  rtems_test_assert( retval == (void *) 456 );

  status = pthread_kill( task_id, SIGUSR1 );
  rtems_test_assert( status == ESRCH );

  task_id = create_task();

  status = pthread_kill( task_id, SIGUSR1 );
  rtems_test_assert( status == 0 );

  status = pthread_join( task_id, &retval );
  rtems_test_assert( status == 0 );
  rtems_test_assert( retval == (void *) 123 );

  TEST_END();
  exit(0);
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INIT_TASK_INITIAL_MODES \
  (RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
