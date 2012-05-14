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

rtems_id         Semaphore_id;
rtems_id         Task_id[OPERATION_COUNT+1];
uint32_t         task_count;
rtems_id         Highest_id;

rtems_task Low_tasks(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Highest_task(
  rtems_task_argument argument
);

rtems_task Restart_task(
  rtems_task_argument argument
);

void test_init(void);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 4 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init(void)
{
  rtems_status_code status;
  int               index;

  task_count = OPERATION_COUNT;

  for ( index = 1 ; index <= OPERATION_COUNT ; index++ ) {

    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      10,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_NO_PREEMPT,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create loop" );

    status = rtems_task_start( Task_id[ index ], Low_tasks, 0 );
    directive_failed( status, "rtems_task_start loop" );
  }

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );
}

rtems_task Highest_task(
  rtems_task_argument argument
)
{
  rtems_task_priority old_priority;
  rtems_status_code   status;

  if ( argument == 1 ) {

    end_time = benchmark_timer_read();

    put_time(
      "rtems_task_restart: blocked task -- preempts caller",
      end_time,
      1,
      0,
      CALLING_OVERHEAD_TASK_RESTART
    );

    status = rtems_task_set_priority(
      RTEMS_CURRENT_PRIORITY,
      RTEMS_MAXIMUM_PRIORITY - 1u,
      &old_priority
    );
    directive_failed( status, "rtems_task_set_priority" );

 } else if ( argument == 2 ) {

  end_time = benchmark_timer_read();

    put_time(
      "rtems_task_restart: ready task -- preempts caller",
      end_time,
      1,
      0,
      CALLING_OVERHEAD_TASK_RESTART
    );

    status = rtems_task_delete( RTEMS_SELF );
    directive_failed( status, "rtems_task_delete of RTEMS_SELF" );

  } else
    (void) rtems_semaphore_obtain(
      Semaphore_id,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );

}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  uint32_t      index;
  rtems_name          name;
  rtems_task_priority old_priority;

  benchmark_timer_initialize();
    (void) rtems_task_restart( Highest_id, 1 );
  /* preempted by Higher_task */

  benchmark_timer_initialize();
    (void) rtems_task_restart( Highest_id, 2 );
  /* preempted by Higher_task */

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      rtems_semaphore_release( Semaphore_id );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_semaphore_release: task readied -- returns to caller",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_SEMAPHORE_RELEASE
  );

  name = rtems_build_name( 'T', 'I', 'M', 'E' );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_delete( Task_id[index] );
    directive_failed( status, "rtems_task_delete" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
     rtems_task_create(
        name,
        10,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_NO_PREEMPT,
        RTEMS_DEFAULT_ATTRIBUTES,
        &Task_id[ index ]
      );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_create",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_CREATE
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      rtems_task_start( Task_id[ index ], Low_tasks, 0 );

  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_start",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_START
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_delete( Task_id[ index ] );
    directive_failed( status, "rtems_task_delete" );
  }

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      name,
      RTEMS_MAXIMUM_PRIORITY - 4u,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_NO_PREEMPT,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create LOOP 1" );

    status = rtems_task_start( Task_id[ index ], Restart_task, 0 );
    directive_failed( status, "rtems_task_start LOOP 1" );

    status = rtems_task_suspend( Task_id[ index ] );
    directive_failed( status, "rtems_task_suspend LOOP 1" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_restart( Task_id[ index ], 0 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_restart: suspended task -- returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_RESTART
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ )
    (void) rtems_task_suspend( Task_id[ index ] );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_delete( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_delete: suspended task",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_DELETE
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      name,
      RTEMS_MAXIMUM_PRIORITY - 4u,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create LOOP 2" );

    status = rtems_task_start( Task_id[ index ], Restart_task, 0 );
    directive_failed( status, "rtems_task_start LOOP 2" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_restart( Task_id[ index ], 1 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_restart: ready task -- returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_RESTART
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_set_priority( Task_id[ index ], 5, &old_priority );
    directive_failed( status, "rtems_task_set_priority loop" );
  }

  /* yield processor -- tasks block */
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_restart( Task_id[ index ], 1 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_restart: blocked task -- returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_RESTART
  );

  /* yield processor -- tasks block */
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_delete( Task_id[ index ] );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_delete: blocked task",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_DELETE
  );

  puts( "*** END OF TEST 4 ***" );
  rtems_test_exit( 0 );
}

rtems_task Low_tasks(
  rtems_task_argument argument
)
{
  rtems_id          id;
  rtems_status_code status;

  task_count--;

  if ( task_count == 0 ) {
    status = rtems_task_create(
      rtems_build_name( 'H', 'I', ' ', ' ' ),
      5,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create HI" );

    status = rtems_task_start( id, High_task, 0 );
    directive_failed( status, "rtems_task_start HI" );

    status = rtems_task_create(
      rtems_build_name( 'H', 'I', 'G', 'H' ),
      3,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Highest_id
    );
    directive_failed( status, "rtems_task_create HIGH" );

    status = rtems_task_start( Highest_id, Highest_task, 0 );
    directive_failed( status, "rtems_task_start HIGH" );

  }
  (void) rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
}

rtems_task Restart_task(
  rtems_task_argument argument
)
{
  if ( argument == 1 )
    (void) rtems_semaphore_obtain(
      Semaphore_id,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
}
