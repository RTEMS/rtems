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

rtems_id Test_task_id;

rtems_task test_task(
  rtems_task_argument argument
);
rtems_task test_task1(
  rtems_task_argument argument
);
void test_init(void);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 8 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init(void)
{
  rtems_status_code status;

  status = rtems_task_create(
    1,
    (RTEMS_MAXIMUM_PRIORITY / 2u) + 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Test_task_id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( Test_task_id, test_task, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_create(
    1,
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Test_task_id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( Test_task_id, test_task1, 0 );
  directive_failed( status, "rtems_task_start" );
}

rtems_task test_task(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  uint32_t      index;
  rtems_task_priority old_priority;
  rtems_time_of_day   time;
  uint32_t      old_note;
  uint32_t      old_mode;

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_set_priority(
               Test_task_id,
               RTEMS_CURRENT_PRIORITY,
               &old_priority
             );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_set_priority: obtain current priority",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_SET_PRIORITY
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_set_priority(
        Test_task_id,
        RTEMS_MAXIMUM_PRIORITY - 2u,
        &old_priority
      );

  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_set_priority: returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_SET_PRIORITY
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_mode(
        RTEMS_CURRENT_MODE,
        RTEMS_CURRENT_MODE,
        &old_mode
      );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_mode: obtain current mode",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_MODE
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
      (void) rtems_task_mode(
        RTEMS_INTERRUPT_LEVEL(1),
        RTEMS_INTERRUPT_MASK,
        &old_mode
      );
      (void) rtems_task_mode(
        RTEMS_INTERRUPT_LEVEL(0),
        RTEMS_INTERRUPT_MASK,
        &old_mode
      );
    }
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_mode: no reschedule",
    end_time,
    OPERATION_COUNT * 2,
    overhead,
    CALLING_OVERHEAD_TASK_MODE
  );

  benchmark_timer_initialize();                 /* must be one host */
    (void) rtems_task_mode( RTEMS_NO_ASR, RTEMS_ASR_MASK, &old_mode );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_mode: reschedule -- returns to caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_TASK_MODE
  );

  status = rtems_task_mode( RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &old_mode );
  directive_failed( status, "rtems_task_mode" );

  status = rtems_task_set_priority( Test_task_id, 1, &old_priority );
  directive_failed( status, "rtems_task_set_priority" );

  /* preempted by test_task1 */
  benchmark_timer_initialize();
    (void)  rtems_task_mode( RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &old_mode );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_set_note( Test_task_id, 8, 10 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_set_note",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_SET_NOTE
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_get_note( Test_task_id, 8, &old_note );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_get_note",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_GET_NOTE
  );

  build_time( &time, 1, 1, 1988, 0, 0, 0, 0 );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_clock_set( &time );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_clock_set",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_CLOCK_SET
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_clock_get_tod( &time );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_clock_get_tod",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_CLOCK_GET
  );

  puts( "*** END OF TEST 8 ***" );
  rtems_test_exit( 0 );
}

rtems_task test_task1(
  rtems_task_argument argument
)
{
  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_mode: reschedule -- preempts caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_TASK_MODE
  );

  (void) rtems_task_suspend( RTEMS_SELF );
}
