/*
 *  COPYRIGHT (c) 1989-2012.
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
#include "test_support.h"

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

  puts( "\n\n*** TEST SP72 ***" );

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

  puts( "*** END OF TEST SP72 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             2
#define CONFIGURE_MAXIMUM_SEMAPHORES        1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
