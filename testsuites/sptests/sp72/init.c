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

const char rtems_test_name[] = "SP 72";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Test_task(rtems_task_argument argument);

rtems_id Semaphore_id;

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "Task - rtems_semaphore_obtain - wait w/timeout " );
  status = rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    rtems_clock_get_ticks_per_second() * 10
  );
  directive_failed( status, "obtain" );

}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          task_id;

  TEST_BEGIN();

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    RTEMS_PRIORITY,
    0,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create - priority blocking" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create" );
  
  status = rtems_task_start( task_id, Test_task, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Init - sleep 1 second" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Init - rtems_task_delete - to extract priority w/timeout" );
  status = rtems_task_delete( task_id );
  directive_failed( status, "rtems_task_delete" );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             2
#define CONFIGURE_MAXIMUM_SEMAPHORES        1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
