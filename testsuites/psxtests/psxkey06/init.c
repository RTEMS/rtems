/*
 *  Copyright (c) 2012 Zhongwei Yao.
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

const char rtems_test_name[] = "PSXKEY 6";

static int Data_array[4] = {1, 2, 3, 4};

static pthread_key_t key1, key2, key3;

static rtems_id Thread_Master;

static int Key3_Destructor_Counter;

static void Wake_Up_Master(void)
{
  rtems_status_code rc;

  rc = rtems_event_transient_send( Thread_Master );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );
}

static void Wait_For_Worker(void)
{
  rtems_status_code rc;

  rc = rtems_event_transient_receive(
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );
}

static rtems_task Test_Thread1( rtems_task_argument argument )
{
  int sc;
  int *value;
  struct timespec  delay_request;

  puts( "Test_Thread 1 - key1 pthread_setspecific - OK" );
  sc = pthread_setspecific( key1, &Data_array[0] );
  rtems_test_assert( !sc );

  puts( "Test_Thread 1 - key2 pthread_setspecific - OK" );
  sc = pthread_setspecific( key2, &Data_array[1] );
  rtems_test_assert( !sc );

  puts( "Test_Thread 1 - sleep - let thread2 run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 4 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Test_Thread 1 - key1 pthread_getspecific - OK" );
  value = pthread_getspecific( key1 );
  rtems_test_assert( *value == Data_array[0] );

  puts( "Test_Thread 1 - key2 pthread_getspecific - OK" );
  value = pthread_getspecific( key2 );
  rtems_test_assert( *value == Data_array[1] );

  rtems_task_exit();
}

static rtems_task Test_Thread2( rtems_task_argument argument )
{
  int sc;
  int *value;

  puts( "Test_Thread 2 - key1 pthread_setspecific - OK" );
  sc = pthread_setspecific( key1, &Data_array[2] );
  rtems_test_assert( !sc );

  puts( "Test_Thread 2 - key2 pthread_setspecific - OK" );
  sc = pthread_setspecific( key2, &Data_array[3] );
  rtems_test_assert( !sc );

  puts( "Test_Thread 2 - key1 pthread_getspecific - OK" );
  value = pthread_getspecific( key1 );
  rtems_test_assert( *value == Data_array[2] );

  puts( "Test_Thread 2 - key2 pthread_getspecific - OK" );
  value = pthread_getspecific( key2 );
  rtems_test_assert( *value == Data_array[3] );

  rtems_task_exit();
}

static void Key3_Destructor( void *value )
{
  rtems_test_assert( value == &Thread_Master );
  ++Key3_Destructor_Counter;
}

static rtems_task Test_Thread3( rtems_task_argument argument )
{
  int   sc;
  void *value;

  puts( "Test_Thread 3 - key3 pthread_getspecific - OK" );
  value = pthread_getspecific( key3 );
  rtems_test_assert( value == NULL );

  puts( "Test_Thread 3 - key3 pthread_setspecific - OK" );
  sc = pthread_setspecific( key3, &Thread_Master );
  rtems_test_assert( sc == 0 );

  if ( argument == 0 ) {
    puts( "Test_Thread 3 - restart self - OK" );
    rtems_task_restart( RTEMS_SELF, 1 );
  } else if ( argument == 1 ) {
    Wake_Up_Master();
    rtems_task_exit();
  }

  rtems_test_assert( false );
}

static rtems_task Init( rtems_task_argument ignored )
{
  rtems_id          thread1;
  rtems_id          thread2;
  rtems_id          thread3;
  rtems_status_code rc;
  int               sc;
  struct timespec   delay_request;

  TEST_BEGIN();

  Thread_Master = rtems_task_self();

  puts( "Init - pthread key1 create - OK" );
  sc = pthread_key_create( &key1, NULL );
  rtems_test_assert( !sc );

  puts( "Init - pthread key2 create - OK" );
  sc = pthread_key_create( &key2, NULL );
  rtems_test_assert( !sc );

  puts( "Init - thread1 create - OK" );
  rc = rtems_task_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ), 
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &thread1
  );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  rc = rtems_task_start( thread1, Test_Thread1, 0 );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  puts( "Init - thread2 create - OK" );
  rc = rtems_task_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ), 
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &thread2
  );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  rc = rtems_task_start( thread2, Test_Thread2, 0 );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  puts( "Init - sleep - let thread run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 8 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - pthread key1 delete - OK" );
  sc = pthread_key_delete( key1 );
  rtems_test_assert( sc == 0 );

  puts( "Init - pthread key2 delete - OK" );
  sc = pthread_key_delete( key2 );
  rtems_test_assert( sc == 0 );

  puts( "Init - pthread key3 create - OK" );
  sc = pthread_key_create( &key3, Key3_Destructor );
  rtems_test_assert( sc == 0 );

  puts( "Init - thread3 create - OK" );
  rc = rtems_task_create(
    rtems_build_name( 'R', 'E', 'S', 'T' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &thread3
  );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  puts( "Init - thread3 start - OK" );
  rc = rtems_task_start( thread3, Test_Thread3, 0 );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  Wait_For_Worker();

  rtems_test_assert( Key3_Destructor_Counter == 2 );

  puts( "Init - pthread key3 delete - OK" );
  sc = pthread_key_delete( key3 );
  rtems_test_assert( sc == 0 );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS          3
#define CONFIGURE_MAXIMUM_POSIX_KEYS     2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
