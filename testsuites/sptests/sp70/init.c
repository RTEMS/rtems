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
void create_helper(int task);
void delete_helper(int task);
 
rtems_id          TaskID[10];

void create_helper(int task)
{
  rtems_status_code status;

  printf( "Creating task %d - OK\n", task );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', task + 0x30, ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &TaskID[task]
  );
  directive_failed( status, "rtems_task_create" );
}

void delete_helper(int task)
{
  rtems_status_code status;

  printf( "Deleting task %d\n", task );
  status = rtems_task_delete( TaskID[task] );
  directive_failed( status, "rtems_task_delete" );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TEST 70 ***" );

  create_helper( 1 );
  create_helper( 2 );
  create_helper( 3 );
  create_helper( 4 );
  create_helper( 5 );
  create_helper( 6 );

  delete_helper( 2 );
  delete_helper( 3 );
  delete_helper( 4 );
  delete_helper( 5 );

  create_helper( 2 );
  create_helper( 3 );
  create_helper( 4 );
  create_helper( 5 );

  puts( "*** END OF TEST 70 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(1)
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
