/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/cpuuse.h>

rtems_task Periodic_Task(
  rtems_task_argument argument
);
rtems_task Init(
  rtems_task_argument argument
);

volatile int partial_loop = 0;

rtems_task Periodic_Task(
  rtems_task_argument argument
)
{
  rtems_status_code  status;
  rtems_name         period_name = rtems_build_name('P','E','R','a');
  rtems_id           period_id;
  rtems_interval     start;
  rtems_interval     end;

  puts( "Periodic - Create Period" );
  /* create period */
  status = rtems_rate_monotonic_create( period_name, &period_id );
  directive_failed(status, "rate_monotonic_create");

  partial_loop = 0;
  while (1) {
    /* start period with initial value */
    status = rtems_rate_monotonic_period( period_id, 25 );
    directive_failed(status, "rate_monotonic_period");
    partial_loop = 0;

    start = rtems_clock_get_ticks_since_boot();
    end   = start + 5;
    while ( end <= rtems_clock_get_ticks_since_boot() )
      ;

    partial_loop = 1;

    rtems_task_wake_after( 5 );
  }

  puts( "Periodic - Deleting self" );
  rtems_task_delete( RTEMS_SELF );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code  status;
  rtems_id           task_id;

  puts( "\n\n*** TEST 46 ***" );

  /*
   * Initialize Tasks
   */


  puts( "INIT - rtems_task_create - creating task 1" );
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of TA1" );

  puts( "INIT - rtems_task_start - TA1 " );
  status = rtems_task_start( task_id, Periodic_Task, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  while ( !partial_loop ) {
    status = rtems_task_wake_after( 2 );
    directive_failed( status, "rtems_task_wake_after" );
  }

  rtems_cpu_usage_reset();

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  /*
   *  Exit test
   */
  puts( "*** END OF TEST 46 *** " );
  rtems_test_exit( 0 );
}

#define CONFIGURE_INIT
/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* Two Tasks: Init and Timer Server */
#define CONFIGURE_MAXIMUM_TASKS           2
#define CONFIGURE_MAXIMUM_PERIODS         1
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)
#define CONFIGURE_INIT_TASK_PRIORITY      10
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS       (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

