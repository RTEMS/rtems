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

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code   status;

  puts( "\n\n*** SIMPLE SCHEDULER 03 TEST ***" );

  puts( "INIT - rtems timer initiate server");
  status = rtems_timer_initiate_server( 
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed( status, "Timer Initiate Server" );

  /*  End the Test */
  puts( "*** END OF SIMPLE SCHEDULER 03 TEST ***" );
  rtems_test_exit(0);
}

/* configuration information */
#define CONFIGURE_SCHEDULER_SIMPLE
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             2
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_PRIORITY        2

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of include file */
