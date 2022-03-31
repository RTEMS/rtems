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

#include <tmacros.h>

const char rtems_test_name[] = "SP 76";

static rtems_task High_task(
  rtems_task_argument index
)
{
  rtems_status_code  status;
  rtems_name         name;

  status = rtems_object_get_classic_name( rtems_task_self(), &name );
  directive_failed( status, "rtems_object_get_classic_name" );

  put_name( name, FALSE );
  puts( " - Successfully yielded it to higher priority task" );

  TEST_END();
  rtems_test_exit( 0 );
}

static rtems_task Equal_task(
  rtems_task_argument index
)
{
  rtems_test_assert( 0 );
}

static rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code     status;
  rtems_id              id;
  rtems_task_priority 	old;

  TEST_BEGIN();

  status = rtems_task_ident( RTEMS_WHO_AM_I, RTEMS_SEARCH_ALL_NODES, &id );
  directive_failed( status, "task ident" );

  /* to make sure it is equal to TA2 */
  puts( "Set Init task priority = 2" );
  status = rtems_task_set_priority( id, 2, &old );
  directive_failed( status, "task priority" );

  puts( "Create TA1 at higher priority task" );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "create 1" );

  status = rtems_task_start( id, High_task, 1 );
  directive_failed( status, "start 1" );

  puts( "Create TA2 at equal priority task" );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '2', ' ' ),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "create 2" );

  status = rtems_task_start( id, Equal_task, 1 );
  directive_failed( status, "start 2" );

  puts( "Yield to TA1" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "yield" );

  puts( "*** should now get here ***" );
}

/* configuration information */
#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           3
#define CONFIGURE_INIT_TASK_PRIORITY      2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
