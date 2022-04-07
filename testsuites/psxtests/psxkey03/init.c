/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

const char rtems_test_name[] = "PSXKEY 3";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument value );
rtems_task Test_Thread( rtems_task_argument value );

void destructor(void *value);

pthread_key_t Key;
volatile bool destructor_ran;

void destructor(void *value)
{
  destructor_ran = true;
}

rtems_task Test_Thread( rtems_task_argument value )
{
  int   sc;
  void *key_value = (void *) value;

  puts( "Test_Thread - pthread_setspecific - OK" );
  sc = pthread_setspecific( Key, key_value );
  rtems_test_assert( !sc );

  puts( "Test_Thread - pthread_exit to run key destructors - OK" );
  rtems_task_exit();
}

rtems_task Init(rtems_task_argument ignored)
{
  rtems_id          thread;
  rtems_status_code rc;
  int               sc;
  struct timespec   delay_request;

  TEST_BEGIN();

  /*
   *  Key with NULL destructor
   */
  puts( "Init - pthread_key_create with NULL destructor - OK" );
  sc = pthread_key_create( &Key, NULL );
  rtems_test_assert( !sc );

  puts( "Init - create/start - OK" );
  rc = rtems_task_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ), 
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &thread
  );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  rc = rtems_task_start( thread, Test_Thread, 0 );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

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

  puts( "Init - task create/start - OK" );
  rc = rtems_task_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &thread
  );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  rc = rtems_task_start( thread, Test_Thread, 0 );
  rtems_test_assert( rc == RTEMS_SUCCESSFUL );

  puts( "Init - sleep - let thread run - OK" );
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  puts( "Init - verify destructor did NOT ran" );
  rtems_test_assert( destructor_ran == false );

  puts( "Init - pthread_key_delete - OK" );
  sc = pthread_key_delete( Key );
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
