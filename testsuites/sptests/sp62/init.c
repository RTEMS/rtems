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
#include <unistd.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Blocker(rtems_task_argument ignored);

rtems_id      Region;
uint32_t      Region_Memory[256] CPU_STRUCTURE_ALIGNMENT;
volatile bool case_hit;

#define    SECOND_ALLOC (sizeof(Region_Memory) / 2)
#define    RESIZE 1

rtems_task Blocker(
  rtems_task_argument ignored
)
{
  rtems_status_code  sc;
  void              *segment_address_2;

  puts( "Blocker - rtems_region_get_segment - OK");
  sc = rtems_region_get_segment(
    Region,
    SECOND_ALLOC,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT,
    &segment_address_2
  );
  directive_failed( sc, "rtems_region_get_segment" );

  sc = rtems_region_return_segment( Region, segment_address_2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Blocker - Got memory after resize" );
  case_hit = true;

  (void) rtems_task_delete( RTEMS_SELF );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_id           task_id;
  rtems_status_code  sc;
  void              *segment_address_1;
  uintptr_t          old_size;
  size_t             first_alloc_size;

  puts( "\n\n*** TEST 62 ***" );

  puts( "Init - rtems_task_create Blocker - OK" );
  sc = rtems_task_create(
    rtems_build_name( 'B', 'L', 'C', 'K' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( sc, "rtems_task_create of Blocker" );

  puts( "Init - rtems_task_start Blocker - OK" );
  sc = rtems_task_start( task_id, Blocker, 0 );
  directive_failed( sc, "rtems_task_start of Blocker" );

  puts( "Init - rtems_task_create Region - OK" );
  sc = rtems_region_create(
    rtems_build_name( 'R', 'N', '1', ' ' ),
    Region_Memory,
    sizeof(Region_Memory),
    16,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Region
  );
  directive_failed( sc, "rtems_region_create" );

  puts( "Init - rtems_region_get_segment - OK");
  first_alloc_size = sizeof( Region_Memory );
  do {
    --first_alloc_size;
    sc = rtems_region_get_segment(
      Region,
      first_alloc_size,
      RTEMS_NO_WAIT,
      RTEMS_NO_TIMEOUT,
      &segment_address_1
    );
  } while ( sc == RTEMS_UNSATISFIED || sc == RTEMS_INVALID_SIZE );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Init - sleep 1 second for Blocker - OK");
  sleep(1);

  /* Check resize_segment errors */
  sc = rtems_region_resize_segment(
    Region, segment_address_1, RESIZE, &old_size);
  directive_failed( sc, "rtems_region_resize_segment" );

  case_hit = false;

  puts( "Init - sleep 1 second for Blocker to run again - OK");
  sleep(1);

  if ( case_hit ) {
    puts( "Init - successfully resized and unblocked a task" );
  } else {
    puts( "Init - failed to resize and unblock a task" );
    rtems_test_exit(0);
  }

  /*
   *  Now resize and take all of memory so there is no need to
   *  process any blocked tasks waiting for memory.
   */
  puts( "Init - resized to all of available memory" );
  sc = rtems_region_resize_segment(
    Region,
    segment_address_1,
    first_alloc_size,
    &old_size
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "*** END OF TEST 62 ***" );
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
