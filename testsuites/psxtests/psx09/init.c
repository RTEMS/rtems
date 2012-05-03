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

#include <sched.h>

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include "pritime.h"

void print_schedparam(
  char               *prefix,
  struct sched_param *schedparam
);

int HIGH_PRIORITY;
int MEDIUM_PRIORITY;
int LOW_PRIORITY;

void print_schedparam(
  char               *prefix,
  struct sched_param *schedparam
)
{
  printf( "%ssched priority      = %d\n", prefix, schedparam->sched_priority );
#if defined(_POSIX_SPORADIC_SERVER)
  printf( "%ssched_ss_low_priority     = %d\n",
      prefix, schedparam->sched_ss_low_priority );
  printf( "%ssched_ss_repl_period = (%" PRIdtime_t ", %ld)\n", prefix,
     schedparam->sched_ss_repl_period.tv_sec,
     schedparam->sched_ss_repl_period.tv_nsec );
  printf( "%ssched_ss_init_budget = (%" PRIdtime_t ", %ld)\n", prefix,
     schedparam->sched_ss_init_budget.tv_sec,
     schedparam->sched_ss_init_budget.tv_nsec );
#else
  printf( "%s_POSIX_SPORADIC_SERVER is not defined\n" );
#endif
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

  puts( "\n\n*** POSIX TEST 9 ***" );

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

  schedparam.sched_priority = sched_get_priority_max(SCHED_SPORADIC);
  schedparam.sched_ss_low_priority = sched_get_priority_max(SCHED_SPORADIC) - 2;

  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( !status );

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  /* go into a loop consuming CPU time to watch our priority change */

  for ( passes=0 ; passes <= 3 ; ) {
    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    rtems_test_assert( !status );

    if ( priority != schedparam.sched_priority ) {
      priority = schedparam.sched_priority;
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

  HIGH_PRIORITY = sched_get_priority_max( SCHED_SPORADIC );
  MEDIUM_PRIORITY = sched_get_priority_max( SCHED_SPORADIC ) - 2;
  LOW_PRIORITY = sched_get_priority_max( SCHED_SPORADIC ) - 4;

  schedparam.sched_priority = HIGH_PRIORITY;
  schedparam.sched_ss_low_priority = LOW_PRIORITY;

  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( !status );

  puts( "Init: Initializing mutex attributes for priority ceiling" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
  rtems_test_assert( !status );

  puts( "Init: Creating a mutex" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  /* go into a loop consuming CPU time to watch our priority NOT lower */

  start = time( &start );

  puts( "Init: pthread_mutex_lock acquire the lock" );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status )
    printf( "status = %d %s\n", status, strerror(status) );
  rtems_test_assert( !status );

  for ( ; ; ) {
    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    rtems_test_assert( !status );

    if ( schedparam.sched_priority == LOW_PRIORITY ) {
      puts( "ERROR - Init's priority lowered while holding mutex" );
      rtems_test_exit(0);
    }

    now = time( &now );
    if ( now - start > 3 )
      break;

    priority = schedparam.sched_priority;
    sprintf( buffer, " - new priority = %d", priority );
    print_current_time( "Init: ", buffer );

    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    rtems_test_assert( !status );

    priority = schedparam.sched_priority;
    sprintf( buffer, " - new priority = %d", priority );
    print_current_time( "Init: ", buffer );

    break;
  }

  /* with this unlock we should be able to go to low priority */

  puts( "Init: unlock mutex" );
  status = pthread_mutex_unlock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  for ( ; ; ) {
    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    rtems_test_assert( !status );

    if ( schedparam.sched_priority == LOW_PRIORITY )
      break;
  }

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  puts( "*** END OF POSIX TEST 9 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
