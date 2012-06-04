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
rtems_task Test_task(rtems_task_argument argument);
void ObtainRelease(bool suspendIdle);

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */
rtems_id   Task_id[ 3 ];         /* array of task ids */
rtems_name Task_name[ 3 ];       /* array of task names */
rtems_name Semaphore_name[ 2 ];
rtems_id   Semaphore_id[ 2 ];

rtems_task Test_task(
  rtems_task_argument unused
)
{
  rtems_id          tid;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "wake after" );

  for ( ; ; ) {
    status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
    directive_failed( status, "yield" );
  }
}

void ObtainRelease( bool suspendIdle )
{
  rtems_status_code   status;

  if (suspendIdle) {
    puts( "INIT - Suspend Idle Task");
    status = rtems_task_suspend( _Thread_Idle->Object.id );
    directive_failed( status, "rtems_task_suspend idle" );
  }

  puts( "INIT - Obtain priority ceiling semaphore - priority increases" );
  status= rtems_semaphore_obtain( Semaphore_id[1], RTEMS_DEFAULT_OPTIONS, 0 );
  directive_failed( status, "rtems_semaphore_obtain" );

  puts( "INIT - Obtain priority ceiling semaphore - priority decreases" );
  status = rtems_semaphore_release( Semaphore_id[1] );
  directive_failed( status, "rtems_semaphore_release" );

  if (suspendIdle) {
    puts( "INIT - Resume Idle Task");
    status = rtems_task_resume( _Thread_Idle->Object.id );
    directive_failed( status, "rtems_task_resume idle" );
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code   status;

  puts( "\n\n*** SIMPLE SCHEDULER 02 TEST ***" );

  /*
   * Create the semaphore. Then obtain and release the
   * semaphore with no other tasks running.
   */
  puts( "INIT - Create priority ceiling semaphore" );
  Semaphore_name[ 1 ] = rtems_build_name( 'S', 'M', '1', ' ' );
  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING | RTEMS_PRIORITY,
    2,
    &Semaphore_id[ 1 ]
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );
  ObtainRelease( false );

  /*
   * Create test task and obtain release the semaphore with
   * one other task running.
   */
  puts( "INIT - create task 1" );
  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  status = rtems_task_create(
    Task_name[ 1 ], 1, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 1 ]
  );
  status = rtems_task_start( Task_id[ 1 ], Test_task, 1 );
  ObtainRelease( false );

  /*
   * Create a a second test task and obtain release the semaphore
   * with both tasks running.
   */
  puts( "INIT - create task 2" );
  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  status = rtems_task_create(
    Task_name[ 2 ], 1, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 2 ]
  );
  status = rtems_task_start( Task_id[ 2 ], Test_task, 1 );
  ObtainRelease( false );

  /*
   * Obtain and release the semaphore with the idle task suspended.
   */
  ObtainRelease( true );

  /*  End the Test */
  puts( "*** END OF SIMPLE SCHEDULER 02 TEST ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_SCHEDULER_SIMPLE
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             3
#define CONFIGURE_MAXIMUM_SEMAPHORES        2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (3 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_INIT_TASK_PRIORITY        4

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of include file */
