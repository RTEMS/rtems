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

rtems_id Test_task_id;

rtems_task test_task(
  rtems_task_argument argument
);
rtems_task test_task1(
  rtems_task_argument argument
);
void test_init();

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

void test_init()
{
  rtems_status_code status;

  status = rtems_task_create(
    1,
    128,
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
    254,
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
  rtems_unsigned32    index;
  rtems_task_priority old_priority;
  rtems_time_of_day   time;
  rtems_unsigned32    old_note;
  rtems_unsigned32    old_mode;

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_set_priority(
               Test_task_id,
               RTEMS_CURRENT_PRIORITY,
               &old_priority
             );
  end_time = Read_timer();

  put_time(
    "rtems_task_set_priority: obtain current priority",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_SET_PRIORITY
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_set_priority( Test_task_id, 253, &old_priority );
  end_time = Read_timer();

  put_time(
    "rtems_task_set_priority: returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_SET_PRIORITY
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_mode(
        RTEMS_CURRENT_MODE,
        RTEMS_CURRENT_MODE,
        &old_mode
      );
  end_time = Read_timer();

  put_time(
    "rtems_task_mode: obtain current mode",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_MODE
  );

  Timer_initialize();
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
  end_time = Read_timer();

  put_time(
    "rtems_task_mode: no reschedule",
    end_time,
    OPERATION_COUNT * 2,
    overhead,
    CALLING_OVERHEAD_TASK_MODE
  );

  Timer_initialize();                 /* must be one host */
    (void) rtems_task_mode( RTEMS_NO_ASR, RTEMS_ASR_MASK, &old_mode );
  end_time = Read_timer();

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
  Timer_initialize();
    (void)  rtems_task_mode( RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &old_mode );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_set_note( Test_task_id, 8, 10 );
  end_time = Read_timer();

  put_time(
    "rtems_task_set_note",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_SET_NOTE
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_task_get_note( Test_task_id, 8, &old_note );
  end_time = Read_timer();

  put_time(
    "rtems_task_get_note",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TASK_GET_NOTE
  );

  build_time( &time, 1, 1, 1988, 0, 0, 0, 0 );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_clock_set( &time );
  end_time = Read_timer();

  put_time(
    "rtems_clock_set",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_CLOCK_SET
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_clock_get( RTEMS_CLOCK_GET_TOD, &time );
  end_time = Read_timer();

  put_time(
    "rtems_clock_get",
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
  end_time = Read_timer();

  put_time(
    "rtems_task_mode: reschedule -- preempts caller",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_TASK_MODE
  );

  (void) rtems_task_suspend( RTEMS_SELF );
}
