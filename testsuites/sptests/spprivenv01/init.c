/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include <tmacros.h>
#include "test_support.h"
#include <rtems/libio_.h>
#include <rtems/malloc.h>
#include <rtems/libcsupport.h>

const char rtems_test_name[] = "SPPRIVENV 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task task_routine(rtems_task_argument not_used);

rtems_task task_routine(rtems_task_argument not_used)
{
  rtems_status_code sc;

  puts( "task_routine - setting up a private environment" );

  sc = rtems_libio_set_private_env();
  directive_failed( sc, "set private env" );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( rtems_current_user_env != &rtems_global_user_env );

  sleep( 1 );

  rtems_task_exit();
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code       sc;
  void                   *opaque;
  rtems_id                task_id;
  rtems_name              another_task_name;
  rtems_user_env_t       *current_env;

  TEST_BEGIN();

  puts( "Init - allocating most of heap -- OK" );
  opaque = rtems_heap_greedy_allocate( NULL, 0 );

  puts( "Init - attempt to reset env - expect RTEMS_NO_MEMORY" );
  sc = rtems_libio_set_private_env();
  rtems_test_assert( sc == RTEMS_NO_MEMORY );

  puts( "Init - freeing the allocated memory" );
  rtems_heap_greedy_free( opaque );

  puts( "Init - allocating most of workspace memory" );
  opaque = rtems_workspace_greedy_allocate( NULL, 0 );

  puts( "Init - attempt to reset env - expect RTEMS_SUCCESSFUL" );
  sc = rtems_libio_set_private_env();
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( rtems_current_user_env != &rtems_global_user_env );

  puts( "Init - freeing the workspace memory" );
  rtems_workspace_greedy_free( opaque );

  puts( "Init - Reset to global environment" );
  rtems_libio_use_global_env();
  rtems_test_assert( rtems_current_user_env == &rtems_global_user_env );

  puts( "Init - Attempt to get a private environment" );
  sc = rtems_libio_set_private_env();
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  current_env = rtems_current_user_env;
  rtems_test_assert( current_env != &rtems_global_user_env );

  puts( "Init - creating a task name and a task -- OK" );

  another_task_name = 
    rtems_build_name( 'T','S','K','D' );

  sc = rtems_task_create( another_task_name,
			  1,
			  RTEMS_MINIMUM_STACK_SIZE * 2,
			  RTEMS_DEFAULT_MODES,
			  RTEMS_DEFAULT_ATTRIBUTES,
			  &task_id
			  );

  puts( "Init - starting the task_routine, to set its private environment" );
  sc = rtems_task_start( task_id, task_routine, 0);
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sleep( 1 );

  puts( "Init - Check current private environment. Should be same as before." );
  rtems_test_assert( rtems_current_user_env == current_env );

  puts( "Init - Reset to global environment" );
  rtems_libio_use_global_env();
  rtems_test_assert( rtems_current_user_env == &rtems_global_user_env );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             3
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
