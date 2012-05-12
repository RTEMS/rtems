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

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Blocking_task(rtems_task_argument ignored);

#define ALLOC_SIZE 400
uint8_t Region_Memory[512] CPU_STRUCTURE_ALIGNMENT;
rtems_id Region;

rtems_task Blocking_task(
  rtems_task_argument ignored
)
{
  rtems_status_code    status;
  void                *address_1;

  puts( "Blocking_task - wait for memory" );
  status = rtems_region_get_segment(
    Region,
    ALLOC_SIZE,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  puts( "Blocking_task - Got memory segment after freed" );

  puts( "Blocking_task - delete self" );
  status = rtems_task_delete(RTEMS_SELF);
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     status;
  rtems_id              task_id;
  void                 *address_1;
  rtems_task_priority   priority;

  puts( "\n\n*** TEST 59 ***" );

  priority = RTEMS_MAXIMUM_PRIORITY / 4;
  priority = (priority * 3) + (priority / 2);
  printf(
    "Init - blocking task priority will be %" PRIdrtems_task_priority "\n",
     priority
  );

  puts( "Init - rtems_task_create - delay task - OK" );
  status = rtems_task_create(
     rtems_build_name( 'T', 'A', '1', ' ' ),
     priority,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_OPTIONS,
     RTEMS_DEFAULT_ATTRIBUTES,
     &task_id
  );
  directive_failed( status, "rtems_task_create" );

  puts( "Init - rtems_task_start - delay task - OK" );
  status = rtems_task_start( task_id, Blocking_task, 0 );
  directive_failed( status, "rtems_task_start" );

  puts( "Init - rtems_region_create - OK" );
  status = rtems_region_create(
    rtems_build_name('R', 'N', '0', '1'),
    Region_Memory,
    sizeof( Region_Memory ),
    64,
    RTEMS_PRIORITY,
    &Region
  );
  directive_failed( status, "rtems_region_create of RN1" );

  puts( "Init - rtems_region_get_segment - get segment to consume memory" );
  rtems_region_get_segment(
    Region,
    ALLOC_SIZE,
    RTEMS_PRIORITY,
    RTEMS_NO_TIMEOUT,
    &address_1
  );
  directive_failed( status, "rtems_region_get_segment" );

  puts( "Init - rtems_task_wake_after - let other task block - OK" );
  status = rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(1000) );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "Init - rtems_region_get_segment - return segment" );
  status = rtems_region_return_segment( Region, address_1 );
  directive_failed( status, "rtems_region_return_segment" );

  puts( "Init - rtems_task_wake_after - let other task run again - OK" );
  status = rtems_task_wake_after( RTEMS_MILLISECONDS_TO_TICKS(1000) );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "*** END OF TEST 59 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_MAXIMUM_REGIONS       1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
