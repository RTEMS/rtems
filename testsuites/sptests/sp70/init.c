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
#include "test_support.h"

const char rtems_test_name[] = "SP 70";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void create_helper(int task);
void delete_helper(int task);
 
rtems_id          TaskID[10];

void create_helper(int task)
{
  rtems_status_code status;

  printf( "Creating task %d - OK\n", task );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', task + 0x30, ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &TaskID[task]
  );
  directive_failed( status, "rtems_task_create" );
}

void delete_helper(int task)
{
  rtems_status_code status;

  printf( "Deleting task %d\n", task );
  status = rtems_task_delete( TaskID[task] );
  directive_failed( status, "rtems_task_delete" );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  TEST_BEGIN();

  create_helper( 1 );
  create_helper( 2 );
  create_helper( 3 );
  create_helper( 4 );
  create_helper( 5 );
  create_helper( 6 );

  delete_helper( 2 );
  delete_helper( 3 );
  delete_helper( 4 );
  delete_helper( 5 );

  create_helper( 2 );
  create_helper( 3 );
  create_helper( 4 );
  create_helper( 5 );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(1)
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
