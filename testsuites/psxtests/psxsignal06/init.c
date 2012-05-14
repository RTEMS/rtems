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

#include <tmacros.h>
#include "test_support.h"
#include <pthread.h>
#include <errno.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void Handler(int signo);
void *TestThread(void *argument);

pthread_t       ThreadId;
pthread_cond_t  CondVarId;
pthread_mutex_t MutexId;

void Handler(
  int signo
)
{
}

void *TestThread(
  void *argument
)
{
  int               status;
  sigset_t          mask;
  struct sigaction  act;
  unsigned int      left;

  /* unblock SIGUSR1 */
  status = sigemptyset( &mask );
  rtems_test_assert(  !status );

  status = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert(  !status );

  puts( "Test: Unblock SIGUSR1" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert(  !status );

  /* install a signal handler for SIGUSR1 */
  act.sa_handler = Handler;
  act.sa_flags   = 0;
  sigaction( SIGUSR1, &act, NULL );

  /* interrupting condition wait returns 0 */
  puts( "Test: pthread_cond_wait - OK" );
  status = pthread_cond_wait( &CondVarId, &MutexId );
  rtems_test_assert( !status );
  puts( "Test: pthread_cond_wait - interrupted by signal" );

  left = sleep( 10 );
  printf( "Test: seconds left=%d\n", left );

  return NULL;
}

void *POSIX_Init(void *argument)
{
  int status;

  puts( "\n\n*** POSIX TEST SIGNAL 06 ***" );

  puts( "Init: pthread_cond_init - OK" );
  status = pthread_cond_init( &CondVarId, NULL );
  rtems_test_assert(  !status );

  puts( "Init: pthread_mutex_init - OK" );
  status = pthread_mutex_init( &MutexId, NULL );
  rtems_test_assert(  !status );

  puts( "Init: pthread_create - OK" );
  status = pthread_create( &ThreadId, NULL, TestThread, NULL );
  rtems_test_assert( !status );

  sleep( 1 );

     /* let TestThread run */

  puts( "Init: pthread_kill - SIGUSR to Test Thread - OK" );
  status = pthread_kill( ThreadId, SIGUSR1 );
  rtems_test_assert( !status );

  sleep( 2 );

     /* let TestThread run */

  puts( "Init: pthread_kill - SIGUSR to Test Thread - OK" );
  status = pthread_kill( ThreadId, SIGUSR1 );
  rtems_test_assert( !status );

  sleep( 1 );

     /* let TestThread run */



  puts( "*** END OF POSIX TEST SIGNAL 06 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS             2
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 2
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES             1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
