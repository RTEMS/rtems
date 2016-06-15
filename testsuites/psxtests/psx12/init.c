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
#include <errno.h>

#include <pmacros.h>

const char rtems_test_name[] = "PSX 12";

static void *sporadic_server( void *argument )
{
  puts( "Sporadic Server: exitting" );

  return NULL;
}

static void *POSIX_Init( void *argument )
{
  int                 status;
  pthread_attr_t      attr;
  pthread_t           thread;
  struct sched_param  schedparam;

  TEST_BEGIN();

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", pthread_self() );

  /* invalid scheduling policy error */

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( !status );
  attr.schedpolicy = -1;

  puts( "Init: pthread_create - EINVAL (invalid scheduling policy)" );
  status = pthread_create( &thread, &attr, sporadic_server, NULL );
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
  status = pthread_create( &thread, &attr, sporadic_server, NULL );
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
  status = pthread_create( &thread, &attr, sporadic_server, NULL );
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
  status = pthread_create( &thread, &attr, sporadic_server, NULL );
  rtems_test_assert( !status );

  status = pthread_join( thread, NULL );
  rtems_test_assert( !status );

    /* switch to Task_1 */

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
