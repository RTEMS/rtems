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
#include <errno.h>

void print_schedparam(
  char               *prefix,
  struct sched_param *schedparam
)
{
  printf( "%ssched priority      = %d\n", prefix, schedparam->sched_priority );
#if defined(_POSIX_SPORADIC_SERVER)
  printf( "%sss_low_priority     = %d\n", prefix, schedparam->ss_low_priority );
  printf( "%sss_replenish_period = (%ld, %ld)\n", prefix,
     schedparam->ss_replenish_period.tv_sec,
     schedparam->ss_replenish_period.tv_nsec );
  printf( "%sss_initial_budget = (%ld, %ld)\n", prefix,
     schedparam->ss_initial_budget.tv_sec,
     schedparam->ss_initial_budget.tv_nsec );
#else
  printf( "%s_POSIX_SPORADIC_SERVER is not defined\n" );
#endif
}

void *POSIX_Init(
  void *argument
)
{
  int                  status;
  int                  schedpolicy;
  int                  priority;
  struct sched_param   schedparam;
  char                 buffer[ 80 ];
  pthread_mutexattr_t  attr;

  puts( "\n\n*** POSIX TEST 9 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );
  
#if 0
  /* try to use this thread as a sporadic server */

  puts( "Init: pthread_getschedparam - SUCCESSFUL" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - current priority = %d", priority );
  print_current_time( "Init: ", buffer );

  schedparam.ss_replenish_period.tv_sec = 0;
  schedparam.ss_replenish_period.tv_nsec = 500000000;  /* 1/2 second */
  schedparam.ss_initial_budget.tv_sec = 0;
  schedparam.ss_initial_budget.tv_nsec = 250000000;    /* 1/4 second */

  schedparam.sched_priority = 200;
  schedparam.ss_low_priority = 100;

  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  assert( !status );

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  /* go into a loop consuming CPU time to watch our priority change */

  for ( passes=0 ; passes <= 3 ; ) {
    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    assert( !status );
 
    if ( priority != schedparam.sched_priority ) {
      priority = schedparam.sched_priority;
      sprintf( buffer, " - new priority = %d", priority );
      print_current_time( "Init: ", buffer );
      passes++;
    }
  }

#endif
  /* now see if this works if we are holding a priority ceiling mutex */

  empty_line();


  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  assert( !status );

  schedparam.ss_replenish_period.tv_sec = 0;
  schedparam.ss_replenish_period.tv_nsec = 500000000;  /* 1/2 second */
  schedparam.ss_initial_budget.tv_sec = 0;
  schedparam.ss_initial_budget.tv_nsec = 250000000;    /* 1/4 second */

#define HIGH_PRIORITY 150
#define MEDIUM_PRIORITY 131
#define LOW_PRIORITY 100

  schedparam.sched_priority = HIGH_PRIORITY;
  schedparam.ss_low_priority = LOW_PRIORITY;
 
  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  assert( !status );

  puts( "Init: Initializing mutex attributes for priority ceiling" );
  status = pthread_mutexattr_init( &attr );
  assert( !status );

  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_PROTECT );
  assert( !status );
 
  status = pthread_mutexattr_setprioceiling( &attr, MEDIUM_PRIORITY );
  assert( !status );
 
  puts( "Init: Creating a mutex" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  assert( !status );
 
  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  /* go into a loop consuming CPU time to watch our priority lower */

  for ( ; ; ) {
    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    assert( !status );

    if ( schedparam.sched_priority != LOW_PRIORITY )
      continue;

    priority = schedparam.sched_priority;
    sprintf( buffer, " - new priority = %d", priority );
    print_current_time( "Init: ", buffer );

    puts( "Init: pthread_mutex_lock acquire the lock" );
    status = pthread_mutex_lock( &Mutex_id );
    if ( status )
      printf( "status = %d\n", status );
    assert( !status );

    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    assert( !status );

    priority = schedparam.sched_priority;
    sprintf( buffer, " - new priority = %d", priority );
    print_current_time( "Init: ", buffer );

    break;
  }

  /* now spin waiting for our budget to be replenished */

  for ( ; ; ) {
    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    assert( !status );
 
    if ( schedparam.sched_priority == HIGH_PRIORITY )
      break;
  }
 
  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );
 
  /* with this unlock we should be able to go to low priority */

  puts( "Init: unlock mutex" ); 
  status = pthread_mutex_unlock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  for ( ; ; ) {
    status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
    assert( !status );
 
    if ( schedparam.sched_priority == LOW_PRIORITY )
      break;
  }

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  assert( !status );
 
  priority = schedparam.sched_priority;
  sprintf( buffer, " - new priority = %d", priority );
  print_current_time( "Init: ", buffer );

  puts( "*** END OF POSIX TEST 9 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
