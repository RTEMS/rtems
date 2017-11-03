/* 
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

const char rtems_test_name[] = "SP 76";

static rtems_task High_task(
  rtems_task_argument index
)
{
  rtems_status_code  status;
  rtems_name         name;

  status = rtems_object_get_classic_name( rtems_task_self(), &name );
  directive_failed( status, "rtems_object_get_classic_name" );

  put_name( name, FALSE );
  puts( " - Successfully yielded it to higher priority task" );

  TEST_END();
  rtems_test_exit( 0 );
}

static rtems_task Equal_task(
  rtems_task_argument index
)
{
  rtems_test_assert( 0 );
}

static rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code     status;
  rtems_id              id;
  rtems_task_priority 	old;

  TEST_BEGIN();

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &id );
  directive_failed( status, "task ident" );

  /* to make sure it is equal to TA2 */
  puts( "Set Init task priority = 2" );
  status = rtems_task_set_priority( id, 2, &old );
  directive_failed( status, "task priority" );

  puts( "Create TA1 at higher priority task" );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "create 1" );

  status = rtems_task_start( id, High_task, 1 );
  directive_failed( status, "start 1" );

  puts( "Create TA2 at equal priority task" );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '2', ' ' ),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "create 2" );

  status = rtems_task_start( id, Equal_task, 1 );
  directive_failed( status, "start 2" );

  puts( "Yield to TA1" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "yield" );

  puts( "*** should now get here ***" );
}

/* configuration information */
#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           3
#define CONFIGURE_INIT_TASK_PRIORITY      2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
