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

#define CONFIGURE_INIT
#include "system.h"

rtems_id Port_id;

uint8_t   Internal_area[ 256 ] CPU_STRUCTURE_ALIGNMENT;
uint8_t   External_area[ 256 ] CPU_STRUCTURE_ALIGNMENT;

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 28 ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( Task_id[ 1 ], Test_task, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Test_task (
  rtems_task_argument argument
)
{
  rtems_name         name;
  uint32_t     index;
  void              *converted;

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  name = rtems_build_name( 'P', 'O', 'R', 'T' ),

  benchmark_timer_initialize();
    rtems_port_create(
      name,
      Internal_area,
      External_area,
      0xff,
      &Port_id
    );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_port_create",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PORT_CREATE
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_port_external_to_internal(
               Port_id,
               &External_area[ 0xf ],
               &converted
             );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_port_external_to_internal",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_PORT_EXTERNAL_TO_INTERNAL
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_port_internal_to_external(
               Port_id,
               &Internal_area[ 0xf ],
               &converted
             );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_port_internal_to_external",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_PORT_INTERNAL_TO_EXTERNAL
  );

  benchmark_timer_initialize();
    rtems_port_delete( Port_id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_port_delete",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PORT_DELETE
  );

  puts( "*** END OF TEST 28 ***" );
  rtems_test_exit( 0 );
}
