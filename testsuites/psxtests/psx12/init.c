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
  int                 status;
  pthread_attr_t      attr;
  struct sched_param  schedparam;

  puts( "\n\n*** POSIX TEST 12 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* invalid scheduling policy error */

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( !status );
  attr.schedpolicy = -1;

  puts( "Init: pthread_create - EINVAL (invalid scheduling policy)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  rtems_test_assert( status == EINVAL );

  /* replenish period < budget error */

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: set scheduling parameter attributes for sporadic server" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_SPORADIC );
  rtems_test_assert( !status );

  schedparam.sched_ss_repl_period.tv_sec = 1;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 2;
  schedparam.sched_ss_init_budget.tv_nsec = 0;

  schedparam.sched_priority = 200;
  schedparam.sched_ss_low_priority = 100;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  rtems_test_assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( !status );

  puts( "Init: pthread_create - EINVAL (replenish < budget)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  rtems_test_assert( status == EINVAL );

  /* invalid sched_ss_low_priority error */

  schedparam.sched_ss_repl_period.tv_sec = 2;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 1;
  schedparam.sched_ss_init_budget.tv_nsec = 0;

  schedparam.sched_priority = 200;
  schedparam.sched_ss_low_priority = -1;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  rtems_test_assert( !status );

  puts( "Init: pthread_create - EINVAL (invalid sched_ss_low_priority)" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  rtems_test_assert( status == EINVAL );

  /* create a thread as a sporadic server */

  schedparam.sched_ss_repl_period.tv_sec = 2;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 1;
  schedparam.sched_ss_init_budget.tv_nsec = 0;

  schedparam.sched_priority = sched_get_priority_max( SCHED_FIFO );
  schedparam.sched_ss_low_priority = sched_get_priority_max( SCHED_FIFO ) - 6;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  rtems_test_assert( !status );

  puts( "Init: pthread_create - SUCCESSFUL" );
  status = pthread_create( &Task_id, &attr, Task_1, NULL );
  rtems_test_assert( !status );

  status = pthread_join( Task_id, NULL );
  rtems_test_assert( status );

    /* switch to Task_1 */

  puts( "*** END OF POSIX TEST 12 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
