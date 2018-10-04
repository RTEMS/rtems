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

const char rtems_test_name[] = "PSXKEY 9";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Test_Thread(rtems_task_argument argument);
void destructor(void *value);

int Data_array[1] = {1};

pthread_key_t key;
volatile bool destructor_ran;

void destructor(void *value)
{
  destructor_ran = true;
}

rtems_task Test_Thread( rtems_task_argument arg )
{
  void *argument = (void *)arg;
  int   sc;

  puts( "Test_Thread - key pthread_setspecific - OK" );
  sc = pthread_setspecific( key, argument );
  rtems_test_assert( !sc );

  puts( "Test_Thread - pthread_exit to run key destructors - OK" );
  rtems_task_exit();
}

rtems_task Init( rtems_task_argument ignored )
{
  rtems_id          thread;
  rtems_status_code rc;
  int               sc;
  struct timespec   delay_request;

  TEST_BEGIN();

  puts( "Init - pthread key create with destructor - OK" );
  sc = pthread_key_create( &key, destructor );
  rtems_test_assert( !sc );

  puts( "Init - thread create - OK" );
  rc = rtems_task_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ), 
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &thread
  );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  rc = rtems_task_start( thread, Test_Thread, (rtems_task_argument)Data_array );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  puts( "Init - sleep - let thread run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 8 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - verify destructor run - OK" );
  rtems_test_assert( destructor_ran == true );

  puts( "Init - pthread key delete - OK" );
  sc = pthread_key_delete( key );
  rtems_test_assert( sc == 0 );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS          2
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
