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

#include <stdio.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>

#include <rtems.h>
#include <tmacros.h>
#include "test_support.h"

#define SIG_SUSPEND SIGUSR1
#define SIG_THR_RESTART SIGUSR2

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
static void print_sig_mask(const char * str);
void GC_suspend_handler(int sig);
void GC_restart_handler(int sig);
void *run(void *arg);

sem_t GC_suspend_ack_sem;

static void print_sig_mask( const char * str )
{
  sigset_t blocked;
  int      i;
  int      status;

  status = pthread_sigmask( SIG_BLOCK, NULL, &blocked );
  rtems_test_assert( status == 0 );

  printf( "%s blocked:\n", str );
  for ( i = 1; i < NSIG; i++) {
    if ( sigismember( &blocked, i ) )
      printf( "%d ", i );
  }
  printf( "\n" );
}

void GC_suspend_handler( int sig )
{
  puts( "run in GC_suspend_handler" );
  sem_post( &GC_suspend_ack_sem );
}

void GC_restart_handler( int sig )
{
  puts( "run in GC_restart_handler" );
}

void *run( void *arg )
{
  int       status;
  pthread_t id = *(pthread_t *)arg;

  print_sig_mask( "New Thread" );

  status = pthread_kill( id, SIG_SUSPEND );
  rtems_test_assert( status == 0 );

  puts( "New Thread: after pthread_kill" );
  status = sem_wait( &GC_suspend_ack_sem );
  rtems_test_assert( status == 0 );

  puts( "New Thread over!" );
  return NULL;
}

void *POSIX_Init( void *arg )
{
  struct sigaction act;
  pthread_t        newThread;
  pthread_t        mainThread;
  int              status;

  puts( "*** POSIX TEST PSXEINTR_JOIN ***" );
  status = sem_init( &GC_suspend_ack_sem, 0, 0);
  rtems_test_assert( status == 0 );

  status = sigemptyset( &act.sa_mask );
  rtems_test_assert( status == 0 );

  status = sigaddset( &act.sa_mask, SIG_SUSPEND );
  rtems_test_assert( status == 0 );

  status = pthread_sigmask( SIG_UNBLOCK, &act.sa_mask, NULL );
  rtems_test_assert( status == 0 );

  act.sa_handler = GC_suspend_handler;

  status = sigaction( SIG_SUSPEND, &act, NULL );
  rtems_test_assert( status == 0 );

  act.sa_handler = GC_restart_handler;

  print_sig_mask( "Main Thread" );

  mainThread = pthread_self();
  status = pthread_create( &newThread, NULL, run, &mainThread );
  rtems_test_assert( status == 0 );

  pthread_join( newThread, NULL );
  puts( "Back from pthread_join" );

  puts( "*** END OF POSIX TEST PSXEINTR_JOIN ***" );
  rtems_test_exit( 0 );

  return NULL;
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_MAXIMUM_POSIX_THREADS 2
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */

