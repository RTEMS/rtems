/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_id          Timer_id[ OPERATION_COUNT+1 ];

rtems_time_of_day time_of_day;

void null_delay(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_task Low_task(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);


rtems_task Init(
  rtems_task_argument argument
)
{

  rtems_task_priority priority;
  rtems_unsigned32    index;
  rtems_id            id;
  rtems_task_entry    task_entry;
  rtems_status_code   status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 23 ***" );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  priority = 5;

  for( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    if ( index == 1 )                    task_entry = High_task;
    else if ( index == OPERATION_COUNT ) task_entry = Low_task;
    else                                 task_entry = Middle_tasks;

    status = rtems_task_start( id, task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );

    priority++;
  }

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void null_delay(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32  index;
  rtems_status_code status;
  int i;

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_create( index, &Timer_id[ index ] );
  end_time = Read_timer();

  put_time(
    "rtems_timer_create",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_CREATE
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_after( Timer_id[ index ], 500, null_delay, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_timer_fire_after: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_FIRE_AFTER
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_after( Timer_id[ index ], 500, null_delay, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_timer_fire_after: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_FIRE_AFTER
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_cancel( Timer_id[ index ] );
  end_time = Read_timer();

  put_time(
    "rtems_timer_cancel: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_CANCEL
  );

  for ( Timer_initialize(), i=0 ; i<OPERATION_COUNT ; i++ )
  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_cancel( Timer_id[ index ] );
  end_time = Read_timer();

  put_time(
    "rtems_timer_cancel: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_CANCEL
  );

  for ( Timer_initialize(), i=0 ; i<OPERATION_COUNT ; i++ )
  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_reset( Timer_id[ index ] );
  end_time = Read_timer();

  put_time(
    "rtems_timer_reset: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_RESET
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_reset( Timer_id[ index ] );
  end_time = Read_timer();

  put_time(
    "rtems_timer_reset: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_RESET
  );

  for ( index=1 ; index <= OPERATION_COUNT ; index++ )
    (void) rtems_timer_reset( Timer_id[ index ] );

  build_time( &time_of_day, 12, 31, 1988, 9, 0, 0, 0 );

  status = rtems_clock_set( &time_of_day );
  directive_failed( status, "rtems_clock_set" );

  time_of_day.year = 1989;

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_when(
         Timer_id[ index ], &time_of_day, null_delay, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_timer_fire_when: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_FIRE_WHEN
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_fire_when(
         Timer_id[ index ], &time_of_day, null_delay, NULL );
  end_time = Read_timer();

  put_time(
    "rtems_timer_fire_when: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_FIRE_WHEN
  );

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_delete( Timer_id[ index ] );
  end_time = Read_timer();

  put_time(
    "rtems_timer_delete: active",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_DELETE
  );

  Timer_initialize();
  for ( index=1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_timer_create( index, &Timer_id[ index ] );
    directive_failed( status, "rtems_timer_create" );

    status = rtems_timer_fire_after( Timer_id[ index ], 500, null_delay, NULL );
    directive_failed( status, "rtems_timer_fire_after" );

    status = rtems_timer_cancel( Timer_id[ index ] );
    directive_failed( status, "rtems_timer_cancel" );
  }

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_timer_delete( Timer_id[ index ] );
  end_time = Read_timer();

  put_time(
    "rtems_timer_delete: inactive",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_TIMER_DELETE
  );

  Timer_initialize();
    (void) rtems_task_wake_when( &time_of_day );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  (void) rtems_task_wake_when( &time_of_day );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  end_time = Read_timer();

  put_time(
    "rtems_task_wake_when",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_TASK_WAKE_WHEN
  );

  puts( "*** END OF TEST 23 ***" );
  rtems_test_exit( 0 );
}
