/**
 * @file spedfsched04/init.c
 *
 * @brief A init task body for spedfsched04 example.
 *
 */

/*
 *  COPYRIGHT (c) 2016 Kuan-Hsun Chen.
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

#include <rtems/rtems/tasksimpl.h>
#include <rtems/test.h>
#include <rtems/status-checks.h>

const char rtems_test_name[] = "SPEDFSCHED 4 - Overrun Test";

/* Global variables */
rtems_id   Task_id[ 2 ];         /* array of task ids */
rtems_name Task_name[ 2 ];       /* array of task names */
uint32_t tick_per_second;        /* time reference */
int testnumber = 5;                  /* stop condition */

rtems_task_priority Prio[ 3 ] = { 0, 2, 5 };

rtems_task Init(
	rtems_task_argument argument
)
{
  uint32_t    index;
  TEST_BEGIN();
  rtems_status_code status;


  tick_per_second = rtems_clock_get_ticks_per_second();
  printf( "\nTicks per second in your system: %" PRIu32 "\n", tick_per_second );

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );

  /* Create two tasks */
  for ( index = 1; index <= 2; index++ ){
    status = rtems_task_create(
      Task_name[ index ], Prio[ index ], RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );
  }

  /* After creating the periods for tasks, start to run them sequencially. */
  for ( index = 1; index <= 2; index++ ){
    status = rtems_task_start( Task_id[ index ], Task, index);
    directive_failed( status, "rtems_task_start loop" );
  }

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

