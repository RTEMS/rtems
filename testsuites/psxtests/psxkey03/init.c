/*
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
void *Test_Thread(void *key_value);

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

  /*
   * Detach ourselves so we don't wait for a join that won't happen.
   */
  pthread_detach( pthread_self() );

  puts( "Test_Thread - pthread_setspecific - OK" );
  sc = pthread_setspecific( Key, key_value );
  rtems_test_assert( !sc );

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
  rtems_test_assert( !sc );

  puts( "Init - pthread_create - OK" );
  sc = pthread_create( &thread, NULL, Test_Thread, &sc );
  rtems_test_assert( !sc );

  puts( "Init - sleep - let thread run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 5 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - pthread_key_delete - OK" );
  sc = pthread_key_delete( Key );
  rtems_test_assert( sc == 0 );

  /*
   *  Key with non-NULL destructor
   */
  destructor_ran = false;
  puts( "Init - pthread_key_create with non-NULL destructor - OK" );
  sc = pthread_key_create( &Key, destructor );
  rtems_test_assert( !sc );

  puts( "Init - pthread_create - OK" );
  sc = pthread_create( &thread, NULL, Test_Thread, NULL );
  rtems_test_assert( !sc );

  puts( "Init - sleep - let thread run - OK" );
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - verify destructor did NOT ran" );
  rtems_test_assert( destructor_ran == false );

  puts( "Init - pthread_key_delete - OK" );
  sc = pthread_key_delete( Key );
  rtems_test_assert( sc == 0 );

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
