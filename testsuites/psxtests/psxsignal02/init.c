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

#include <pmacros.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Test_Thread(void *arg);
void Signal_handler(int signo);
void Install_Signal_Handler(const char *task_name);

volatile bool      Signal_occurred;
volatile pthread_t Signal_thread;

void Signal_handler(
  int signo
)
{
  Signal_occurred = true;
  Signal_thread   = pthread_self();
}

void Install_Signal_Handler(
  const char *task_name
)
{
  int               sc;
  sigset_t          mask;

  sc = sigemptyset( &mask );
  rtems_test_assert( !sc );

  sc = sigaddset( &mask, SIGUSR1 );
  rtems_test_assert( !sc );

  printf( "%s - Unblock SIGUSR1\n", task_name );
  sc = pthread_sigmask( SIG_UNBLOCK, &mask, NULL );
  rtems_test_assert( !sc );
}

/*

Tasks and actions, created in this order, all interested in SIGUSR1
  - 20     - interested, suspend?
  - 18     - interested, suspend?
  - 16     - interested, spins
  - 14     - interested, spins
  - 12     - interested, sleeps
  - 10     - interested, suspends
  - 8      - interested, sleeps

Order is critical because the algorithm works by thread index
*/

typedef enum {
  SUSPEND,
  SPIN,
  SLEEP
} Action_t;

const char *Actions[] = {
  "Suspends self",
  "Spins",
  "Sleeps"
};


typedef struct {
  int         priority;
  Action_t    action;
  const char *name;
} Test_t;

Test_t Threads[] = {
  {  8, SUSPEND,  "P8"  },  /* base priority */
  {  7, SUSPEND,  "P7"  },  /* lower priority -- no change */
  { 12, SUSPEND,  "P12" },  /* higher priority, blocked */
  { 12, SUSPEND,  "P12" },  /* equal priority, blocked */
  { 12, SLEEP,    "P12" },  /* equal priority, interruptible */
  { 12, SLEEP,    "P12" },  /* equal priority, interruptible */
  { 12, SPIN,     "P12" },  /* equal priority, ready */
  { 12, SPIN,     "P12" },  /* equal priority, ready -- no change */
  { -1, 0,        ""    },
};

void *Test_Thread(void *arg)
{
  Test_t *test = (Test_t *)arg;

  Install_Signal_Handler( test->name );

  printf( "%s - %s\n", test->name, Actions[test->action] );
  switch ( test->action ) {
    case SUSPEND:
      (void) rtems_task_suspend( RTEMS_SELF );
      break;
    case SPIN:
      while (1) ;
      break;
    case SLEEP:
      sleep( 30 );
      break;
  }

  printf( "%s - exiting\n", test->name );
  return NULL;

}

void *POSIX_Init(
  void *argument
)
{
  int                 i;
  int                 sc;
  pthread_t           id;
  pthread_attr_t      attr;
  struct sched_param  param;
  Test_t             *test;
  struct sigaction    act;
  struct timespec     delay_request;

  puts( "\n\n*** POSIX TEST SIGNAL 02 ***" );

  Signal_occurred = false;

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
  sigaction( SIGUSR1, &act, NULL );

  puts( "Init - Raise my priority" );
  sc = pthread_attr_init( &attr );
  rtems_test_assert( !sc );

  param.sched_priority = 30;
  sc = pthread_setschedparam( pthread_self(), SCHED_RR, &param );
  rtems_test_assert( !sc );

  for ( i=0, test=Threads ; test->priority != -1 ; i++, test++ ) {
    printf( "Init - Create thread %d, priority=%d\n", i, test->priority );
    sc = pthread_attr_init( &attr );
    rtems_test_assert( !sc );

    sc = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
    rtems_test_assert( !sc );

    sc = pthread_attr_setschedpolicy( &attr, SCHED_RR );
    rtems_test_assert( !sc );

    param.sched_priority = test->priority;
    sc = pthread_attr_setschedparam( &attr, &param );
    rtems_test_assert( !sc );

    sc = pthread_create( &id, &attr, Test_Thread, test );
    rtems_test_assert( !sc );

    puts( "Init - sleep - let thread settle - OK" );
    delay_request.tv_sec = 0;
    delay_request.tv_nsec = 50000000;
    sc = nanosleep( &delay_request, NULL );
    rtems_test_assert( !sc );
  }

  puts( "Init - sending SIGUSR1" );
  sc =  kill( getpid(), SIGUSR1 );
  rtems_test_assert( !sc );

  /* we are just scheduling the signal, not delivering it */
  rtems_test_assert( Signal_occurred == false );

  puts( "*** END OF POSIX TEST SIGNAL 02 ***" );
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK        1000
#define CONFIGURE_MAXIMUM_POSIX_THREADS        9

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
