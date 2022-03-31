/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#include <rtems/libcsupport.h>

#include <tmacros.h>

const char rtems_test_name[] = "SP 18";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_id                task_id;
  rtems_status_code       sc;
  bool                    ok;
  uintptr_t               stack_size;
  rtems_resource_snapshot snapshot;
  void                   *greedy;

  TEST_BEGIN();

  greedy = rtems_workspace_greedy_allocate_all_except_largest( &stack_size );
  rtems_resource_snapshot_take( &snapshot );

  /* Make sure the first allocation fails */
  ++stack_size;

  puts( "Init - rtems_task_create - Unsatisfied on Extensions" );
  while (1) {

    sc = rtems_task_create(
      rtems_build_name( 'T', 'E', 'S', 'T' ),
      1,
      stack_size,
      RTEMS_DEFAULT_MODES,
      RTEMS_FLOATING_POINT,
      &task_id
    );

    if ( sc == RTEMS_SUCCESSFUL )
      break;

    fatal_directive_status( sc, RTEMS_UNSATISFIED, "rtems_task_create" );

    /*
     * Verify heap is still in same shape if we couldn't allocate a task
     */
    ok = rtems_resource_snapshot_check( &snapshot );
    rtems_test_assert( ok );

    stack_size -= 8;
    if ( stack_size <= RTEMS_MINIMUM_STACK_SIZE )
     break;
  }

  if ( sc != RTEMS_SUCCESSFUL )
    rtems_test_exit(0);

  /*
   * Verify heap is still in same shape after we free the task
   */
  puts( "Init - rtems_task_delete - OK" );
  sc = rtems_task_delete( task_id );
  directive_failed( sc, "rtems_task_delete" );

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

#define CONFIGURE_MAXIMUM_TASKS            2
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS  20
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
