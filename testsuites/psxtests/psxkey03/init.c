/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <pthread.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

pthread_key_t Key;
volatile bool destructor_ran;

void destructor(void *value)
{
  destructor_ran = true;
}

void *Test_Thread(
  void *key_value
)
{
  int sc;

  puts( "Test_Thread - pthread_setspecific - OK" );
  sc = pthread_setspecific( Key, key_value );
  assert( !sc );

  puts( "Test_Thread - pthread_exit to run key destructors - OK" );
  return NULL;
}

void *POSIX_Init(
  void *ignored
)
{
  pthread_t        thread;
  int              sc;
  struct timespec  delay_request;

  puts( "\n\n*** TEST KEY 03 ***" );

  /*
   *  Key with NULL destructor
   */
  puts( "Init - pthread_key_create with NULL destructor - OK" );
  sc = pthread_key_create( &Key, NULL );
  assert( !sc );

  puts( "Init - pthread_create - OK" );
  sc = pthread_create( &thread, NULL, Test_Thread, &sc );
  assert( !sc );

  puts( "Init - sleep - let thread run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 5 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  assert( !sc );

  puts( "Init - pthread_key_delete - OK" );
  sc = pthread_key_delete( Key );
  assert( sc == 0 );

  /*
   *  Key with non-NULL destructor
   */
  destructor_ran = false;
  puts( "Init - pthread_key_create with non-NULL destructor - OK" );
  sc = pthread_key_create( &Key, destructor );
  assert( !sc );

  puts( "Init - pthread_create - OK" );
  sc = pthread_create( &thread, NULL, Test_Thread, NULL );
  assert( !sc );

  puts( "Init - sleep - let thread run - OK" );
  sc = nanosleep( &delay_request, NULL );
  assert( !sc );

  puts( "Init - verify destructor did NOT ran" );
  assert( destructor_ran == false );

  puts( "Init - pthread_key_delete - OK" );
  sc = pthread_key_delete( Key );
  assert( sc == 0 );

  puts( "*** END OF TEST KEY 03 ***" );
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
