/*
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include "system.h"
#undef EXTERN
#define EXTERN
#include "conftbl.h"
#include "gvar.h"

rtems_id Semaphore_id;

rtems_task High_tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          task_id;
  rtems_unsigned32  index;
  rtems_status_code status;

  puts( "\n\n*** TIME TEST 25 ***" );

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ') ,
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    254,
    1024,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create LOW" );

  status = rtems_task_start( task_id, Low_task, 0 );
  directive_failed( status, "rtems_task_start LOW" );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      128,
      1024,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( task_id, High_tasks, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task High_tasks(
  rtems_task_argument argument
)
{
  (void) rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    0xffffffff
  );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  Timer_initialize();
    (void) rtems_clock_tick();
  end_time = Read_timer();

  put_time(
    "rtems_clock_tick",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_CLOCK_TICK
  );

  exit( 0 );
}
