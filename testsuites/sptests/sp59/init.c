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

const char rtems_test_name[] = "SP 59";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Blocking_task(rtems_task_argument ignored);

uint8_t Region_Memory[512] CPU_STRUCTURE_ALIGNMENT;
#define ALLOC_SIZE ( sizeof( Region_Memory ) / 2 )
rtems_id Region;

rtems_task Blocking_task(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;
  void                *address_1;

  puts( "Blocking_task - wait for memory" );
  status = rtems_region_get_segment(
    Region,
    ALLOC_SIZE,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  puts( "Blocking_task - Got memory segment after freed" );

  puts( "Blocking_task - delete self" );
  rtems_task_exit();
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     status;
  rtems_id              task_id;
  void                 *address_1;
  rtems_task_priority   priority;

  TEST_BEGIN();

  priority = RTEMS_MAXIMUM_PRIORITY / 4;
  priority = (priority * 3) + (priority / 2);
  printf(
    "Init - blocking task priority will be %" PRIdrtems_task_priority "\n",
     priority
  );

  puts( "Init - rtems_task_create - delay task - OK" );
  status = rtems_task_create(
     rtems_build_name( 'T', 'A', '1', ' ' ),
     priority,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_DEFAULT_ATTRIBUTES,
     &task_id
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Init - rtems_task_start - delay task - OK" );
  status = rtems_task_start( task_id, Blocking_task, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Init - rtems_region_create - OK" );
  status = rtems_region_create(
    rtems_build_name('R', 'N', '0', '1'),
    Region_Memory,
    sizeof( Region_Memory ),
    64,
    RTEMS_PRIORITY,
    &Region
  );
  directive_failed( status, "rtems_region_create of RN1" );

  puts( "Init - rtems_region_get_segment - get segment to consume memory" );
  rtems_region_get_segment(
    Region,
    ALLOC_SIZE,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  puts( "Init - rtems_task_wake_after - let other task block - OK" );
  status = rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(1000) );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Init - rtems_region_return_segment - return segment" );
  status = rtems_region_return_segment( Region, address_1 );
  directive_failed( status, "rtems_region_return_segment" );

  puts( "Init - rtems_task_wake_after - let other task run again - OK" );
  status = rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(1000) );
  directive_failed( status, "rtems_task_wake_after" );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_MAXIMUM_REGIONS       1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
