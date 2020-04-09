/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP 1";

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT ) static char Task_1_storage[
  RTEMS_TASK_STORAGE_SIZE( 2 * RTEMS_MINIMUM_STACK_SIZE, RTEMS_FLOATING_POINT )
];

static const rtems_task_config Task_1_config = {
  .name = rtems_build_name( 'T', 'A', '1', ' ' ),
  .initial_priority = 1,
  .storage_area = Task_1_storage,
  .storage_size = sizeof( Task_1_storage ),
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_FLOATING_POINT
};

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code status;
  rtems_id          id;

  TEST_BEGIN();

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  status = rtems_task_construct( &Task_1_config, &id );
  directive_failed( status, "rtems_task_construct of TA1" );

  status = rtems_task_start( id, Task_1_through_3, 1 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '2', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_start( id, Task_1_through_3, 2 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '3', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE * 3,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_start( id, Task_1_through_3, 3 );
  directive_failed( status, "rtems_task_start of TA3" );

  rtems_task_exit();
}
