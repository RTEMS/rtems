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
void destructor(void *value);
void *Test_Thread(void *argument);

int Data_array[1] = {1};

pthread_key_t key;
volatile bool destructor_ran;

void destructor(void *value)
{
  destructor_ran = true;
}

void *Test_Thread(
  void *argument
)
{
  int sc;

  /**
   * Detach ourselves to release test thread's resource after thread exit.
   */
  pthread_detach( pthread_self() );

  puts( "Test_Thread - key pthread_setspecific - OK" );
  sc = pthread_setspecific( key, argument );
  rtems_test_assert( !sc );

  puts( "Test_Thread - pthread key delete - OK" );
  sc = pthread_key_delete( key );
  rtems_test_assert( sc == 0 );

  puts( "Test_Thread - pthread exit, but don't run key destructors - OK" );
  return NULL;
}

void *POSIX_Init(
  void *ignored
)
{
  pthread_t        thread;
  int              sc;
  struct timespec  delay_request;

  puts( "\n\n*** TEST KEY 10 ***" );

  puts( "Init - pthread key create with destructor - OK" );
  sc = pthread_key_create( &key, destructor );
  rtems_test_assert( !sc );

  puts( "Init - pthread create - OK" );
  sc = pthread_create( &thread, NULL, Test_Thread, &sc );
  rtems_test_assert( !sc );

  puts( "Init - sleep - let thread run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 8 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - verify destructor did NOT run - OK" );
  rtems_test_assert( destructor_ran == false );

  /* puts( "Init - pthread key delete - OK" ); */
  /* sc = pthread_key_delete( key ); */
  /* rtems_test_assert( sc == 0 ); */

  puts( "*** END OF TEST KEY 10 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS  2
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
