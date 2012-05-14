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

rtems_id Task_id[ OPERATION_COUNT + 1 ];

uint32_t   Task_restarted;

rtems_task null_task(
  rtems_task_argument argument
);

rtems_task Task_1(
  rtems_task_argument argument
);

void test_init( void );

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 6 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init( void )
{
  rtems_status_code status;
  rtems_id          id;

  Task_restarted = OPERATION_COUNT;

  status = rtems_task_create(
    rtems_build_name( 'T', 'I', 'M', 'E' ),
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, Task_1, 0 );
  directive_failed( status, "rtems_task_start" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  uint32_t    index;

  if ( Task_restarted == OPERATION_COUNT )
     benchmark_timer_initialize();

  Task_restarted--;

  if ( Task_restarted != 0 )
    (void) rtems_task_restart( RTEMS_SELF, 0 );

  end_time = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  put_time(
    "rtems_task_restart: calling task",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_RESTART
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      RTEMS_MAXIMUM_PRIORITY - 1u,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );

    status = rtems_task_start( Task_id[ index ], null_task, 0 );
    directive_failed( status, "rtems_task_start loop" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_suspend( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_suspend: returns to caller",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_TASK_SUSPEND
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_resume( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_resume: task readied -- returns to caller",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_TASK_RESUME
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_delete( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_delete: ready task",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_TASK_RESUME
  );

  puts( "*** END OF TEST 6 ***" );
  rtems_test_exit( 0 );
}

rtems_task null_task(
  rtems_task_argument argument
)
{
  while ( FOREVER )
    ;
}
