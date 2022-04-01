/* SPDX-License-Identifier: BSD-2-Clause */

/*  Init
 *
 *  This routine is the initialization routine for this test program.
 *  Other than creating all objects needed by this test, if this routine
 *  is running on node one, it acquires a global semaphore to
 *  force all other tasks to pend.  If running on node two, this task
 *  sleeps for a while, and then deletes two local tasks which are
 *  waiting on a remote message queue or a semaphore.
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
  rtems_status_code status;

  printf(
   "\n\n*** TEST 10 -- NODE %" PRIu32 " ***\n",
   rtems_object_get_local_node()
  );

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] =  rtems_build_name( 'S', 'A', '3', ' ' );

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
      0,
      RTEMS_GLOBAL | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &Semaphore_id[ 1 ]
    );
    directive_failed( status, "rtems_semaphore_create" );

    status = rtems_task_wake_after( 10 * rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

  } else {

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

    puts( "Creating Test_task 3 (local)" );
    status = rtems_task_create(
      Task_name[ 3 ],
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_TIMESLICE,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ 3 ]
    );
    directive_failed( status, "rtems_task_create" );

    puts( "Starting Test_task 3 (local)" );
    status = rtems_task_start( Task_id[ 3 ], Test_task2, 0 );
    directive_failed( status, "rtems_task_start" );

    puts( "Sleeping for 1 seconds ..." );
    status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    puts( "Deleting Test_task2" );
    status = rtems_task_delete( Task_id[ 2 ] );
    directive_failed( status, "rtems_task_delete of Task 2" );

    puts( "Deleting Test_task1" );
    status = rtems_task_delete( Task_id[ 1 ] );
    directive_failed( status, "rtems_task_delete of Task 1" );

    puts( "Restarting Test_task3" );
    status = rtems_task_restart( Task_id[ 3 ], 1 );
    directive_failed( status, "rtems_task_restart of Task 3" );

  }
  puts( "*** END OF TEST 10 ***" );
  rtems_test_exit( 0 );
}
