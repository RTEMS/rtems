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

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "PSXONCE 1";

static pthread_once_t nesting_once = PTHREAD_ONCE_INIT;

static void Test_init_routine_nesting( void )
{
  int status;
  puts( "Test_init_routine_nesting: invoked" );
  puts( "Test_init_routine_nesting: pthread_once - EINVAL (init_routine_nesting does not execute)" );
  status = pthread_once( &nesting_once, Test_init_routine_nesting );
  rtems_test_assert( status == EINVAL );
}

static int test_init_routine_call_counter = 0;

static void Test_init_routine( void )
{
  puts( "Test_init_routine: invoked" );
  ++test_init_routine_call_counter;
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

  puts( "Init: pthread_once - SUCCESSFUL (init_routine_nesting executes)" );
  status = pthread_once( &nesting_once, Test_init_routine_nesting );
  rtems_test_assert( !status );

  TEST_END();
  rtems_test_exit( 0 );
}
