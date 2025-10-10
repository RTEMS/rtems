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

/*
 * This used to be a configuration option.  If there is a real need, it can be
 * made a configuration option again.
 */
#define MAX_LONG_TEST_DURATION 100

uint8_t   my_partition[0x30000] CPU_STRUCTURE_ALIGNMENT;

static rtems_timer_service_routine Stop_Test_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  (void) ignored_id;
  (void) ignored_address;

  Stop_Test = true;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code   status;
  rtems_task_priority previous_priority;

  printf(
    "\n\n*** TEST 14 -- NODE %" PRId32 " ***\n",
    rtems_object_get_local_node()
  );

  Stop_Test = false;

  status = rtems_timer_create(
    rtems_build_name('S', 'T', 'O', 'P'),
    &timer_id
  );
  directive_failed( status, "rtems_timer_create" );

  status = rtems_timer_fire_after(
    timer_id,
    MAX_LONG_TEST_DURATION * rtems_clock_get_ticks_per_second(),
    Stop_Test_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  Task_name[ 1 ] = rtems_build_name( '1', '1', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( '2', '2', '2', ' ' );

  Queue_task_name[ 1 ] = rtems_build_name( 'M', 'T', '1', ' ' );
  Queue_task_name[ 2 ] = rtems_build_name( 'M', 'T', '2', ' ' );

  Partition_task_name[ 1 ] = rtems_build_name( 'P', 'T', '1', ' ' );
  Partition_task_name[ 2 ] = rtems_build_name( 'P', 'T', '2', ' ' );

  Semaphore_task_name[ 1 ] = rtems_build_name( 'S', 'M', '1', ' ' );
  Semaphore_task_name[ 2 ] = rtems_build_name( 'S', 'M', '2', ' ' );

  Semaphore_name[ 1 ] =  rtems_build_name( 'S', 'E', 'M', ' ' );

  Queue_name[ 1 ] = rtems_build_name( 'M', 'S', 'G', ' ' );

  Partition_name[ 1 ] = rtems_build_name( 'P', 'A', 'R', ' ' );

  Timer_name[ 1 ] = rtems_build_name( 'T', 'M', 'R', ' ' );

  if ( rtems_object_get_local_node() == 1 ) {
    puts( "Creating Semaphore (Global)" );
    status = rtems_semaphore_create(
      Semaphore_name[ 1 ],
      1,
      RTEMS_GLOBAL,
      RTEMS_NO_PRIORITY,
      &Semaphore_id[ 1 ]
    );
    directive_failed( status, "rtems_semaphore_create" );

    puts( "Creating Message Queue (Global)" );
    status = rtems_message_queue_create(
      Queue_name[ 1 ],
      1,
      16,
      RTEMS_GLOBAL,
      &Queue_id[ 1 ]
    );
    directive_failed( status, "rtems_message_queue_create" );

    puts( "Creating Partition (Global)" );
    status = rtems_partition_create(
      Partition_name[ 1 ],
      (void *)my_partition,
      0x8000,
      0x3800,
      RTEMS_GLOBAL,
      &Partition_id[ 1 ]
    );
    directive_failed( status, "rtems_partition_create" );
  }

  puts( "Creating Event task (Global)" );
  status = rtems_task_create(
    Task_name[ rtems_object_get_local_node() ],
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_GLOBAL,
    &Event_task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Starting Event task (Global)" );
  status = rtems_task_start( Event_task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Creating Semaphore task (Global)" );
  status = rtems_task_create(
    Semaphore_task_name[ rtems_object_get_local_node() ],
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_GLOBAL,
    &Semaphore_task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Starting Semaphore task (Global)" );
  status = rtems_task_start( Semaphore_task_id[ 1 ], Semaphore_task, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Creating Message Queue task (Global)" );
  status = rtems_task_create(
    Queue_task_name[ rtems_object_get_local_node() ],
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_TIMESLICE,
    RTEMS_GLOBAL,
    &Queue_task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  /* argument is index into Buffers */
  puts( "Starting Message Queue task (Global)" );
  status = rtems_task_start( Queue_task_id[ 1 ], Message_queue_task, 1 );
  directive_failed( status, "rtems_task_start" );

  puts( "Creating Partition task (Global)" );
  status = rtems_task_create(
    Partition_task_name[ rtems_object_get_local_node() ],
    2,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_TIMESLICE,
    RTEMS_GLOBAL,
    &Partition_task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Starting Partition task (Global)" );
  status = rtems_task_start( Partition_task_id[ 1 ], Partition_task, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_set_priority( RTEMS_SELF, 2, &previous_priority );
  directive_failed( status, "rtems_task_set_priority" );

  status = rtems_task_ident(
    RTEMS_SELF,
    RTEMS_SEARCH_ALL_NODES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_ident" );

  Delayed_events_task( 1 );
}
