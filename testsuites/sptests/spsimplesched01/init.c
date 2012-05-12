/* 
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Test_task(rtems_task_argument unused);

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */
rtems_id   Task_id[ 4 ];         /* array of task ids */
rtems_name Task_name[ 4 ];       /* array of task names */

rtems_task Test_task(
  rtems_task_argument unused
)
{
  rtems_id           tid;
  rtems_time_of_day  time;
  uint32_t           task_index;
  rtems_status_code  status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "task ident" );

  task_index = task_number( tid );
  for ( ; ; ) {
    status = rtems_clock_get_tod( &time );
    directive_failed( status, "clock get tod" );
    if ( time.second >= 35 ) {
      puts( "*** END OF SIMPLE01 TEST ***" );
      rtems_test_exit( 0 );
    }
    put_name( Task_name[ task_index ], FALSE );
    print_time( " - rtems_clock_get_tod - ", &time, "\n" );
    status = rtems_task_wake_after(
      task_index * 5 * rtems_clock_get_ticks_per_second()
    );
    directive_failed( status, "wake after" );
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  rtems_time_of_day   time;
  rtems_task_priority old;

  puts( "\n\n*** SIMPLE01 TEST ***" );

  time.year   = 1988;
  time.month  = 12;
  time.day    = 31;
  time.hour   = 9;
  time.minute = 0;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ], 1, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 1 ]
  );
  directive_failed( status, "create 1" );

  status = rtems_task_create(
    Task_name[ 2 ], 1, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 2 ]
  );
  directive_failed( status, "create 2" );

  status = rtems_task_create(
    Task_name[ 3 ], 1, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 3 ]
  );
  directive_failed( status, "create 3" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 1 );
  directive_failed( status, "start 1" );
  rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  status = rtems_task_start( Task_id[ 2 ], Test_task, 2 );
  directive_failed( status, "start 2" );
  rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );

  status = rtems_task_start( Task_id[ 3 ], Test_task, 3 );
  directive_failed( status, "start 3" );

  status = rtems_task_set_priority( Task_id[1], 2, &old );
  directive_failed( status, "set priority 1" );
  status = rtems_task_set_priority( Task_id[2], 2, &old );
  directive_failed( status, "set priority 2" );
  status = rtems_task_set_priority( Task_id[3], 2, &old );
  directive_failed( status, "set priority 3" );

  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "yield" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "delete self" );
}

/* configuration information */
#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_SCHEDULER_SIMPLE
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (3 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
