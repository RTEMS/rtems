/* SPDX-License-Identifier: BSD-2-Clause */

/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
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

#define CONFIGURE_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  rtems_time_of_day time;
  char              c;

  printf(
    "\n\n*** TEST 1 -- NODE %" PRIu32 " ***\n",
    rtems_object_get_local_node()
  );

  if ( rtems_object_get_local_node() != 1 ) c = 'S';
  else                                           c = 'M';

  Task_name[ 1 ] = rtems_build_name( c, 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( c, 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( c, 'A', '3', ' ' );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  puts( "Creating task 1 (Global)" );
  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_GLOBAL,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of Task 1" );

  puts( "Creating task 2 (Global)" );
  status = rtems_task_create(
    Task_name[ 2 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_GLOBAL,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of Task 2" );

  puts( "Creating task 3 (Local)" );
  status = rtems_task_create(
    Task_name[ 3 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of Task 3" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of Task 1" );

  status = rtems_task_start( Task_id[ 2 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of Task 2" );

  status = rtems_task_start( Task_id[ 3 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start of Task 3" );

  rtems_task_exit();
}
