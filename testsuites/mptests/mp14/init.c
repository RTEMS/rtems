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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

uint8_t   my_partition[0x30000] CPU_STRUCTURE_ALIGNMENT;

rtems_timer_service_routine Stop_Test_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  Stop_Test = true;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  rtems_task_priority previous_priority;

  printf(
    "\n\n*** TEST 14 -- NODE %" PRId32 " ***\n",
    Multiprocessing_configuration.node
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

  if ( Multiprocessing_configuration.node == 1 ) {
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
    Task_name[ Multiprocessing_configuration.node ],
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
    Semaphore_task_name[ Multiprocessing_configuration.node ],
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
    Queue_task_name[ Multiprocessing_configuration.node ],
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
    Partition_task_name[ Multiprocessing_configuration.node ],
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
