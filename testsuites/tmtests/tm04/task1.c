/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(OPERATION_COUNT)
#define OPERATION_COUNT 100
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/btimer.h>

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "TIME TEST 4";

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
  (void) argument;

  Print_Warning();

  TEST_BEGIN();

  test_init();

  rtems_task_exit();
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
  (void) argument;

  rtems_task_priority old_priority;
  rtems_status_code   status;

  if ( argument == 1 ) {

    end_time = benchmark_timer_read();

    put_time(
      "rtems_task_restart: blocked task preempts caller",
      end_time,
      1,
      0,
      0
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
      0
    );

    rtems_task_exit();
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
  (void) argument;

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
    0
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
    "rtems_task_create: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

  benchmark_timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      rtems_task_start( Task_id[ index ], Low_tasks, 0 );

  end_time = benchmark_timer_read();

  put_time(
    "rtems_task_start: only case",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
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
    0
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
    0
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
    0
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
    0
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
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}

rtems_task Low_tasks(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_id          id;
  rtems_status_code status;
  rtems_mode        prev;

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

  rtems_task_mode(RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &prev);
}

rtems_task Restart_task(
  rtems_task_argument argument
)
{
  (void) argument;

  if ( argument == 1 )
    (void) rtems_semaphore_obtain(
      Semaphore_id,
      RTEMS_DEFAULT_OPTIONS,
      RTEMS_NO_TIMEOUT
    );
}
