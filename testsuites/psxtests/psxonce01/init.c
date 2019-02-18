/*
 *  Copyright (C) 2019 embedded brains GmbH
 *  Copyright (C) 2019 Sebastian Huber
 *
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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "PSXONCE 1";

static pthread_once_t once_a = PTHREAD_ONCE_INIT;

static pthread_once_t once_b = PTHREAD_ONCE_INIT;

static rtems_id master;

static int test_init_routine_call_counter = 0;

static void Test_init_routine( void )
{
  puts( "Test_init_routine: invoked" );
  ++test_init_routine_call_counter;
}

static void routine_b( void )
{
  rtems_status_code sc;

  rtems_test_assert( test_init_routine_call_counter == 2 );
  ++test_init_routine_call_counter;

  sc = rtems_event_send( master, RTEMS_EVENT_0 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
}

static void use_b( rtems_task_argument arg )
{
  int status;

  (void) arg;

  status = pthread_once( &once_b, routine_b );
  rtems_test_assert( status == 0 );

  rtems_task_exit();
}

static void routine_a( void )
{
  rtems_status_code sc;
  rtems_id id;
  rtems_event_set events;

  rtems_test_assert( test_init_routine_call_counter == 1 );
  ++test_init_routine_call_counter;

  master = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name( 'T', 'A', 'S', 'K' ),
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_start( id, use_b, 0 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  events = 0;
  sc = rtems_event_receive(
    RTEMS_EVENT_0,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( events == RTEMS_EVENT_0 );

  rtems_test_assert( test_init_routine_call_counter == 3 );
}

rtems_task Init(rtems_task_argument argument)
{
  int status;
  pthread_once_t once = PTHREAD_ONCE_INIT;

  TEST_BEGIN();

  puts( "Init: pthread_once - EINVAL (NULL once_control)" );
  status = pthread_once( NULL, Test_init_routine );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_once - EINVAL (NULL init_routine)" );
  status = pthread_once( &once, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_once - SUCCESSFUL (init_routine executes)" );
  status = pthread_once( &once, Test_init_routine );
  rtems_test_assert( !status );
  printf( "Init: call counter: %d\n", test_init_routine_call_counter );
  rtems_test_assert( test_init_routine_call_counter == 1 );

  puts( "Init: pthread_once - SUCCESSFUL (init_routine does not execute)" );
  status = pthread_once( &once, Test_init_routine );
  rtems_test_assert( !status );
  printf( "Init: call counter: %d\n", test_init_routine_call_counter );
  rtems_test_assert( test_init_routine_call_counter == 1 );

  status = pthread_once( &once_a, routine_a );
  rtems_test_assert( status == 0 );
  rtems_test_assert( test_init_routine_call_counter == 3 );

  TEST_END();
  rtems_test_exit( 0 );
}
