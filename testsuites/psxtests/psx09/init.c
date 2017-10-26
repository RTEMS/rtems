/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sched.h>

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include "pritime.h"

const char rtems_test_name[] = "PSX 9";

static int CEILING_PRIORITY;
static int HIGH_PRIORITY;
static int LOW_PRIORITY;

static int get_current_prio( pthread_t thread )
{
  rtems_status_code sc;
  rtems_task_priority prio;
  int max;

  sc = rtems_task_set_priority( thread, RTEMS_CURRENT_PRIORITY, &prio );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  max = sched_get_priority_max( SCHED_FIFO );

  return max + 1 - (int) prio;
}

static void *mutex_lock_task(void *arg)
{
  pthread_mutex_t *mtx;
  int              eno;

  mtx = arg;

  eno = pthread_mutex_lock( mtx );
  rtems_test_assert( eno == 0 );

  sched_yield();

  eno = pthread_mutex_unlock( mtx );
  rtems_test_assert( eno == 0 );

  return NULL;
}

static void test_destroy_locked_mutex(void)
{
  pthread_mutex_t mtx;
  pthread_t       th;
  int             eno;

  eno = pthread_mutex_init( &mtx, NULL );
  rtems_test_assert( eno == 0 );

  eno = pthread_create( &th, NULL, mutex_lock_task, &mtx );
  rtems_test_assert( eno == 0 );

  sched_yield();

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == EBUSY );

  sched_yield();

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_join( th, NULL );
  rtems_test_assert( eno == 0 );
}

void *POSIX_Init(
  void *argument
)
{
  int                  status;
  int                  passes;
  int                  schedpolicy;
  int                  priority;
  struct sched_param   schedparam;
  char                 buffer[ 80 ];
  pthread_mutexattr_t  attr;
  time_t               start;
  time_t               now;

  TEST_BEGIN();

  CEILING_PRIORITY = sched_get_priority_max( SCHED_SPORADIC );
  HIGH_PRIORITY = sched_get_priority_max( SCHED_SPORADIC ) - 1;
  LOW_PRIORITY = sched_get_priority_max( SCHED_SPORADIC ) - 2;

  test_destroy_locked_mutex();

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* try to use this thread as a sporadic server */

  puts( "Init: pthread_getschedparam - SUCCESSFUL" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - current priority = %d", priority );
  print_current_time( "Init: ", buffer );

  schedparam.sched_ss_repl_period.tv_sec = 0;
  schedparam.sched_ss_repl_period.tv_nsec = 500000000;  /* 1/2 second */
  schedparam.sched_ss_init_budget.tv_sec = 0;
  schedparam.sched_ss_init_budget.tv_nsec = 250000000;    /* 1/4 second */

  schedparam.sched_priority = HIGH_PRIORITY;
  schedparam.sched_ss_low_priority = LOW_PRIORITY;

  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( !status );

  sprintf( buffer, " - new priority = %d", get_current_prio( pthread_self() ) );
  print_current_time( "Init: ", buffer );

  /* go into a loop consuming CPU time to watch our priority change */

  for ( passes=0 ; passes <= 3 ; ) {
    int current_priority;

    current_priority = get_current_prio( pthread_self() );

    if ( priority != current_priority ) {
      priority = current_priority;
      sprintf( buffer, " - new priority = %d", priority );
      print_current_time( "Init: ", buffer );
      passes++;
    }
  }

  /* now see if this works if we are holding a priority ceiling mutex */

  empty_line();

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  schedparam.sched_ss_repl_period.tv_sec = 0;
  schedparam.sched_ss_repl_period.tv_nsec = 500000000;  /* 1/2 second */
  schedparam.sched_ss_init_budget.tv_sec = 0;
  schedparam.sched_ss_init_budget.tv_nsec = 250000000;    /* 1/4 second */

  schedparam.sched_priority = HIGH_PRIORITY;
  schedparam.sched_ss_low_priority = LOW_PRIORITY;

  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( !status );

  puts( "Init: Initializing mutex attributes for priority ceiling" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_PROTECT );
  rtems_test_assert( !status );

  status = pthread_mutexattr_setprioceiling( &attr, CEILING_PRIORITY );
  rtems_test_assert( !status );

  puts( "Init: Creating a mutex" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  sprintf( buffer, " - new priority = %d", get_current_prio( pthread_self() ) );
  print_current_time( "Init: ", buffer );

  /* go into a loop consuming CPU time to watch our priority NOT lower */

  start = time( &start );

  puts( "Init: pthread_mutex_lock acquire the lock" );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status )
    printf( "status = %d %s\n", status, strerror(status) );
  rtems_test_assert( !status );

  do {
    priority = get_current_prio( pthread_self() );

    if ( priority != CEILING_PRIORITY ) {
      puts( "ERROR - Init's priority lowered while holding mutex" );
      rtems_test_exit(0);
    }

    now = time( &now );
  } while ( now - start < 3 );

  /* with this unlock we should be able to go to low priority */

  puts( "Init: unlock mutex" );
  status = pthread_mutex_unlock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  sprintf( buffer, " - new priority = %d", get_current_prio( pthread_self() ) );
  print_current_time( "Init: ", buffer );

  for ( ; ; ) {
    if ( get_current_prio( pthread_self() ) == LOW_PRIORITY )
      break;
  }

  sprintf( buffer, " - new priority = %d", get_current_prio( pthread_self() ) );
  print_current_time( "Init: ", buffer );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
