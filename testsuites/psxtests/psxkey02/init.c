/*
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
#include <rtems/libcsupport.h>
#include "tmacros.h"
#include "pmacros.h"

const char rtems_test_name[] = "PSXKEY 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

rtems_task Init(rtems_task_argument ignored)
{
  pthread_key_t           key1;
  pthread_key_t           key2;
  int                     eno;
  bool                    ok;
  rtems_resource_snapshot snapshot;
  void                   *greedy;
  void                   *value;

  TEST_BEGIN();

  greedy = rtems_workspace_greedy_allocate( NULL, 0 );
  rtems_resource_snapshot_take( &snapshot );

  puts( "Init - pthread_key_create - OK" );
  eno = pthread_key_create( &key1, NULL );
  rtems_test_assert( eno == 0 );

  eno = pthread_setspecific( key1, (void *) 1 );
  rtems_test_assert( eno == 0 );

  value = pthread_getspecific( key1 );
  rtems_test_assert( value == (void *) 1 );

  eno = pthread_setspecific( key1, NULL );
  rtems_test_assert( eno == 0 );

  value = pthread_getspecific( key1 );
  rtems_test_assert( value == NULL );

  eno = pthread_setspecific( key1, NULL );
  rtems_test_assert( eno == 0 );

  value = pthread_getspecific( key1 );
  rtems_test_assert( value == NULL );

  puts( "Init - pthread_key_create - EAGAIN" );
  eno = pthread_key_create( &key2, NULL );
  rtems_test_assert( eno == EAGAIN );

  puts( "Init - pthread_key_delete - OK" );
  eno = pthread_key_delete( key1 );
  rtems_test_assert( eno == 0 );

  puts( "Init - verify workspace has same memory" );
  ok = rtems_resource_snapshot_check( &snapshot );
  rtems_test_assert( ok );

  rtems_workspace_greedy_free( greedy );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
