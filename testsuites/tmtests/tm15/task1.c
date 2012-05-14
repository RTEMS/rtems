/*
 *  COPYRIGHT (c) 1989-2011.
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

bool     time_set;
uint32_t eventout;

rtems_task High_tasks(
  rtems_task_argument argument
);

rtems_task Low_task(
  rtems_task_argument argument
);

void test_init(void);

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 15 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init(void)
{
  rtems_id          id;
  uint32_t    index;
  rtems_event_set   event_out;
  rtems_status_code status;

  time_set = false;

  status = rtems_task_create(
    rtems_build_name( 'L', 'O', 'W', ' ' ),
    10,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create LOW" );

  status = rtems_task_start( id, Low_task, 0 );
  directive_failed( status, "rtems_task_start LOW" );

  for ( index = 1 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'H', 'I', 'G', 'H' ),
      5,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ index ]
    );
    directive_failed( status, "rtems_task_create LOOP" );

    status = rtems_task_start( Task_id[ index ], High_tasks, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
    {
        (void) rtems_event_receive(
                 RTEMS_PENDING_EVENTS,
                 RTEMS_DEFAULT_OPTIONS,
                 RTEMS_NO_TIMEOUT,
                 &event_out
               );
    }

  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: obtain current events",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_EVENT_RECEIVE
  );


  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
    {
      (void) rtems_event_receive(
               RTEMS_ALL_EVENTS,
               RTEMS_NO_WAIT,
               RTEMS_NO_TIMEOUT,
               &event_out
             );
    }
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: not available -- NO_WAIT",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_EVENT_RECEIVE
  );
}

rtems_task Low_task(
  rtems_task_argument argument
)
{
  uint32_t    index;
  rtems_event_set   event_out;

  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: not available -- caller blocks",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_EVENT_RECEIVE
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) benchmark_timer_empty_function();
  overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_event_send( RTEMS_SELF, RTEMS_EVENT_16 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_send: no task readied",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_EVENT_SEND
  );

  benchmark_timer_initialize();
    (void) rtems_event_receive(
             RTEMS_EVENT_16,
             RTEMS_DEFAULT_OPTIONS,
             RTEMS_NO_TIMEOUT,
             &event_out
           );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_receive: available",
    end_time,
    1,
    0,
    CALLING_OVERHEAD_EVENT_RECEIVE
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_event_send( Task_id[ index ], RTEMS_EVENT_16 );
  end_time = benchmark_timer_read();

  put_time(
    "rtems_event_send: task readied -- returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_EVENT_SEND
  );

  puts( "*** END OF TEST 15 ***" );
  rtems_test_exit( 0 );
}

rtems_task High_tasks(
  rtems_task_argument argument
)
{
  if ( time_set )
    (void) rtems_event_receive(
      RTEMS_EVENT_16,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &eventout
    );
  else {
    time_set = true;
    /* start blocking rtems_event_receive time */
    benchmark_timer_initialize();
    (void) rtems_event_receive(
      RTEMS_EVENT_16,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT,
      &eventout
    );
  }
}
