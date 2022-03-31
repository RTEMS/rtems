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
#include <unistd.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Task_1(rtems_task_argument arg);

#if defined(INHERIT_CEILING)
  #define TEST_NAME                "66"
  #define TASK_PRIORITY            2
#else
  #define TEST_NAME                "65"
  #define TASK_PRIORITY            1
#endif

const char rtems_test_name[] = "SP " TEST_NAME;

static void assert_priority(rtems_task_priority expected)
{
  rtems_status_code sc;
  rtems_task_priority prio;

  sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(prio == expected);
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  int                  status;
  rtems_id             Mutex_id, Task_id;

  TEST_BEGIN();

  /*
   * Verify that an initially locked priority ceiling mutex elevates the
   * priority of the creating task.
   */

  status = rtems_semaphore_create(
    rtems_build_name( 's','e','m','1' ),
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    1,
    &Mutex_id
  );
  rtems_test_assert(status == RTEMS_SUCCESSFUL);

  assert_priority(1);

  status = rtems_semaphore_release(Mutex_id);
  rtems_test_assert(status == RTEMS_SUCCESSFUL);

  assert_priority(TASK_PRIORITY);

  status = rtems_semaphore_delete(Mutex_id);
  rtems_test_assert(status == RTEMS_SUCCESSFUL);

  /*
   *  Create binary semaphore (a.k.a. Mutex) with Priority Ceiling
   *  attribute.
   */

  puts( "Creating semaphore" );
  status = rtems_semaphore_create(
    rtems_build_name( 's','e','m','1' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    1,
    &Mutex_id
  );
  directive_failed( status, "rtems_semaphore_create" );

  puts( "Calling rtems_semaphore_obtain" );
  status = rtems_semaphore_obtain( Mutex_id, RTEMS_DEFAULT_OPTIONS, 0 );
  directive_failed( status, "rtems_semaphore_obtain" );

  puts( "Calling rtems_task_create" );
  status = rtems_task_create( rtems_build_name( 'T', 'A', 'S', '1' ),
    TASK_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Calling rtems_task_start" );
  status = rtems_task_start( Task_id, Task_1, (rtems_task_argument)&Mutex_id );
  directive_failed( status, "rtems_task_start" );

  sleep(1);

  puts( "Calling semaphore release" );
  status = rtems_semaphore_release( Mutex_id );
  directive_failed( status, "rtems_semaphore_release" );

  TEST_END();

  rtems_test_exit(0);
}

rtems_task Task_1(
  rtems_task_argument arg
)
{
  int status_in_task;
  rtems_id *Mutex_id = (rtems_id *)arg;

  puts( "Init Task_1: Obtaining semaphore" );
  status_in_task = rtems_semaphore_obtain(
    *Mutex_id,
    RTEMS_DEFAULT_OPTIONS,
    0
  );
  directive_failed( status_in_task, "Task_1 rtems_semaphore_obtain" );
  return;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_MAXIMUM_SEMAPHORES    1
#define CONFIGURE_INIT_TASK_PRIORITY    TASK_PRIORITY
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
