/* 
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
  int                 status;
  pthread_attr_t      attr;
  struct sched_param  schedparam;

  puts( "\n\n*** POSIX TEST 12 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );
  
  /* invalid scheduling policy error */

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  assert( !status );
  attr.schedpolicy = -1;

  puts( "Init: pthread_create - EINVAL (invalid scheduling policy)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( status == EINVAL );

  /* replenish period < budget error */

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  assert( !status );

  puts( "Init: set scheduling parameter attributes for sporadic server" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_SPORADIC );
  assert( !status );

  schedparam.ss_replenish_period.tv_sec = 1;
  schedparam.ss_replenish_period.tv_nsec = 0;
  schedparam.ss_initial_budget.tv_sec = 2;
  schedparam.ss_initial_budget.tv_nsec = 0;
 
  schedparam.sched_priority = 200;
  schedparam.ss_low_priority = 100;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  assert( !status );
 
  puts( "Init: pthread_create - EINVAL (replenish < budget)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( status == EINVAL );

  /* invalid ss_low_priority error */

  schedparam.ss_replenish_period.tv_sec = 2;
  schedparam.ss_replenish_period.tv_nsec = 0;
  schedparam.ss_initial_budget.tv_sec = 1;
  schedparam.ss_initial_budget.tv_nsec = 0;
 
  schedparam.sched_priority = 200;
  schedparam.ss_low_priority = -1;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  assert( !status );

  puts( "Init: pthread_create - EINVAL (invalid ss_low_priority)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( status == EINVAL );

  /* create a thread as a sporadic server */

  schedparam.ss_replenish_period.tv_sec = 2;
  schedparam.ss_replenish_period.tv_nsec = 0;
  schedparam.ss_initial_budget.tv_sec = 1;
  schedparam.ss_initial_budget.tv_nsec = 0;
 
  schedparam.sched_priority = 200;
  schedparam.ss_low_priority = 100;
 
  status = pthread_attr_setschedparam( &attr, &schedparam );
  assert( !status );
 
  puts( "Init: pthread_create - SUCCESSFUL" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  assert( !status );

  status = pthread_join( Task_id, NULL );
  assert( status );

    /* switch to Task_1 */

  puts( "*** END OF POSIX TEST 12 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
