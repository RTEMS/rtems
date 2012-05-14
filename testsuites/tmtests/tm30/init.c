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

#include <bsp.h>
#include <coverhd.h>
#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"

rtems_id barrier[ OPERATION_COUNT ];

rtems_task Init(
  rtems_task_argument argument
);

static void benchmark_barrier_create(
  int    iteration,
  void  *argument
)
{
  rtems_status_code status;

  status = rtems_barrier_create(
    iteration + 1,
    RTEMS_LOCAL | RTEMS_FIFO,
    2,
    &barrier[iteration]
  );
  directive_failed(status, "rtems_barrier_create");
}

static void benchmark_barrier_ident(
  int    iteration,
  void  *argument
)
{
  rtems_status_code status;
  rtems_id          id;

  status = rtems_barrier_ident( iteration+1, &id );
  directive_failed(status, "rtems_barrier_ident");
}

static void benchmark_barrier_delete(
  int    iteration,
  void  *argument
)
{
  rtems_status_code status;

  status = rtems_barrier_delete( barrier[iteration] );
  directive_failed(status, "rtems_barrier_delete");
}

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TIME TEST 30 ***" );

  rtems_time_test_measure_operation(
    "rtems_barrier_create",
    benchmark_barrier_create,
    NULL,
    OPERATION_COUNT,
    0
  );
  
  rtems_time_test_measure_operation(
    "rtems_barrier_ident",
    benchmark_barrier_ident,
    NULL,
    OPERATION_COUNT,
    0
  );
  
  rtems_time_test_measure_operation(
    "rtems_barrier_delete",
    benchmark_barrier_delete,
    NULL,
    OPERATION_COUNT,
    0
  );
  
  puts( "*** END OF TIME TEST 30 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_BARRIERS          OPERATION_COUNT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
