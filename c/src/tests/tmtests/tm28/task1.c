/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_id Port_id;

rtems_unsigned8 Internal_area[ 256 ] CPU_STRUCTURE_ALIGNMENT;
rtems_unsigned8 External_area[ 256 ] CPU_STRUCTURE_ALIGNMENT;

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
    128,
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
  rtems_unsigned32   index;
  void              *converted;

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  name = rtems_build_name( 'P', 'O', 'R', 'T' ),

  Timer_initialize();
    rtems_port_create(
      name,
      Internal_area,
      External_area,
      0xff,
      &Port_id
    );
  end_time = Read_timer();

  put_time(
    "rtems_port_create",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_PORT_CREATE
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_port_external_to_internal(
               Port_id,
               &External_area[ 0xf ],
               &converted
             );
  end_time = Read_timer();

  put_time(
    "rtems_port_external_to_internal",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_PORT_EXTERNAL_TO_INTERNAL
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_port_internal_to_external(
               Port_id,
               &Internal_area[ 0xf ],
               &converted
             );
  end_time = Read_timer();

  put_time(
    "rtems_port_internal_to_external",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_PORT_INTERNAL_TO_EXTERNAL
  );

  Timer_initialize();
    rtems_port_delete( Port_id );
  end_time = Read_timer();

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
