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
 *  COPYRIGHT (c) 1989-2009.
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

  printf(
    "\n\n*** TEST 13 -- NODE %" PRId32 " ***\n",
    rtems_object_get_local_node()
  );

  Task_name[ 1 ] = rtems_build_name( '1', '1', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( '2', '2', '2', ' ' );

  Queue_name[ 1 ] = rtems_build_name( 'M', 'S', 'G', ' ' );

  Semaphore_name[ 1 ] = rtems_build_name( 'S', 'E', 'M', ' ' );

  if ( rtems_object_get_local_node() == 1 ) {
    puts( "Creating Message Queue (Global)" );
    status = rtems_message_queue_create(
      Queue_name[ 1 ],
      3,
      16,
      RTEMS_GLOBAL,
      &Queue_id[ 1 ]
    );
    directive_failed( status, "rtems_message_queue_create" );

    puts( "Creating Semaphore (Global)" );
    status = rtems_semaphore_create(
      Semaphore_name[ 1 ],
      1,
      RTEMS_GLOBAL | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &Semaphore_id[ 1 ]
    );
    directive_failed( status, "rtems_semaphore_create" );

    status = rtems_semaphore_obtain(
      Semaphore_id[ 1 ],
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
    directive_failed( status, "rtems_semaphore_obtain" );
  }

  puts( "Creating Test_task 1 (local)" );
  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Starting Test_task 1 (local)" );
  status = rtems_task_start( Task_id[ 1 ], Test_task1, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Creating Test_task 2 (local)" );
  status = rtems_task_create(
    Task_name[ 2 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Starting Test_task 2 (local)" );
  status = rtems_task_start( Task_id[ 2 ], Test_task2, 0 );
  directive_failed( status, "rtems_task_start" );

  if ( rtems_object_get_local_node() == 1 ) {
    status = rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "*** END OF TEST 13 ***" );
    rtems_test_exit( 0 );
  }
  puts( "Deleting initialization task" );
  rtems_task_exit();
}
