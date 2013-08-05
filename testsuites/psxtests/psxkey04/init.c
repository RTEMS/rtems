/*
 *  Copyright (c) 2012 Zhongwei Yao.
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Test_Thread1(void *argument);
void *Test_Thread2(void *argument);

int Data_array[2] = {1, 2};
pthread_t        thread1, thread2;

pthread_key_t Key;

void *Test_Thread1(
  void *argument
)
{
  int sc;
  int *value;
  struct timespec  delay_request;
  /*
   * Detach ourselves so we don't wait for a join that won't happen.
   */
  pthread_detach( pthread_self() );

  puts( "Test_Thread 1 - pthread_setspecific - OK" );
  sc = pthread_setspecific( Key, &Data_array[0] );
  rtems_test_assert( !sc );

  puts( "Test_Thread 1 - sleep - let thread 2 run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 4 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Test_Thread 1 - pthread_getspecific - OK" );
  value = pthread_getspecific( Key );
  rtems_test_assert( *value == Data_array[0] );

  return NULL;
}

void *Test_Thread2(
  void *argument
)
{
  int sc;
  int *value;
  /*
   * Detach ourselves so we don't wait for a join that won't happen.
   */
  pthread_detach( pthread_self() );

  puts( "Test_Thread 2 - pthread_setspecific - OK" );
  sc = pthread_setspecific( Key, &Data_array[1] );
  rtems_test_assert( !sc );

  puts( "Test_Thread 2 - pthread_getspecific - OK" );
  value = pthread_getspecific( Key );
  rtems_test_assert( *value == Data_array[1] );

  return NULL;
}

void *POSIX_Init(
  void *ignored
)
{
  int              sc;
  struct timespec  delay_request;

  puts( "\n\n*** TEST KEY 04 ***" );

  puts( "Init - pthread_key_create - OK" );
  sc = pthread_key_create( &Key, NULL );
  rtems_test_assert( !sc );

  puts( "Init - pthread_create - OK" );
  sc = pthread_create( &thread1, NULL, Test_Thread1, NULL );
  rtems_test_assert( !sc );

  sc = pthread_create( &thread2, NULL, Test_Thread2, NULL );
  rtems_test_assert( !sc );

  puts( "Init - sleep - let thread run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 8 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - pthread_key_delete - OK" );
  sc = pthread_key_delete( Key );
  rtems_test_assert( sc == 0 );

  puts( "*** END OF TEST KEY 04 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS  3
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
