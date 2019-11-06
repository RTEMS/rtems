/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined(OPERATION_COUNT)
#define OPERATION_COUNT 100
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#if defined(TM03)
const char rtems_test_name[] = "TIME TEST 3";
#define SEMAPHORE_ATTRIBUTES (RTEMS_COUNTING_SEMAPHORE | RTEMS_FIFO)
#define ATTR_DESC "counting/FIFO"

#elif defined(TM32)
const char rtems_test_name[] = "TIME TEST 32";
#define SEMAPHORE_ATTRIBUTES (RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY)
#define ATTR_DESC "counting/priority"

#elif defined(TM34)
const char rtems_test_name[] = "TIME TEST 34";
#define SEMAPHORE_ATTRIBUTES RTEMS_BINARY_SEMAPHORE
#define ATTR_DESC "binary/FIFO"

#elif defined(TM36)
const char rtems_test_name[] = "TIME TEST 36";
#define SEMAPHORE_ATTRIBUTES (RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY)
#define ATTR_DESC "binary/priority"

#else
#error "Unknown test configuration"
#endif


rtems_id Semaphore_id;
rtems_task test_init(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

int operation_count = OPERATION_COUNT;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id          task_id;

  Print_Warning();

  TEST_BEGIN();
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    RTEMS_MAXIMUM_PRIORITY - 1u,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of test_init" );

  status = rtems_task_start( task_id, test_init, 0 );
  directive_failed( status, "rtems_task_start of test_init" );

  rtems_task_exit();
}

rtems_task test_init(
  rtems_task_argument argument
)
{
  rtems_status_code   status;
  int                 index;
  rtems_id            task_id;
  rtems_task_priority priority;

  priority = RTEMS_MAXIMUM_PRIORITY - 2u;

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', '\0'),
    0,
    SEMAPHORE_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  if ( OPERATION_COUNT > RTEMS_MAXIMUM_PRIORITY - 2u )
    operation_count =  (int) (RTEMS_MAXIMUM_PRIORITY - 2u);
  for ( index = 2 ; index < operation_count ; index ++ ) {
    rtems_task_create(
      rtems_build_name( 'M', 'I', 'D', ' ' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create middle" );

    priority--;

    rtems_task_start( task_id, Middle_tasks, 0 );
    directive_failed( status, "rtems_task_start middle" );
  }

  status = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create of high task" );

  status = rtems_task_start( task_id, High_task, 0 );
  directive_failed( status, "rtems_task_start of high task" );

  benchmark_timer_initialize();                          /* start the timer */
  status = rtems_semaphore_release( Semaphore_id );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  (void) rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );

  (void) rtems_semaphore_release( Semaphore_id );
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  (void) rtems_semaphore_obtain(
    Semaphore_id,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );

  end_time = benchmark_timer_read();

  put_time(
    "rtems_semaphore_release: " ATTR_DESC " task readied preempts caller",
    end_time,
    operation_count - 1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );
}
