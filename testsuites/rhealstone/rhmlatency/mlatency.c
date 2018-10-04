/*
 * Copyright (c) 2014 Daniel Ramirez. (javamonn@gmail.com)
 *
 * This file's license is 2-clause BSD as in this distribution's LICENSE file.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <timesys.h>
#include <rtems/btimer.h>

const char rtems_test_name[] = "RHMLATENCY";

#define MESSAGE_SIZE (sizeof(long) * 4)
#define BENCHMARKS 50000

rtems_task Init( rtems_task_argument ignored );
rtems_task Task01( rtems_task_argument ignored );
rtems_task Task02( rtems_task_argument ignored );

uint32_t    telapsed;
uint32_t    tloop_overhead;
uint32_t    treceive_overhead;
uint32_t    count;
rtems_id    Task_id[2];
rtems_name  Task_name[2];
rtems_id    Queue_id;
long        Buffer[4];

void Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  TEST_BEGIN();

  status = rtems_message_queue_create(
    rtems_build_name( 'M', 'Q', '1', ' '  ),
    1,
    MESSAGE_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id
  );
  directive_failed( status, "rtems_message_queue_create" );

  Task_name[0] = rtems_build_name( 'T','A','0','1' );
  status = rtems_task_create(
    Task_name[0],
    30,               /* TA01 is low priority task */
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[0]
  );
  directive_failed( status, "rtems_task_create of TA01");

  Task_name[1] = rtems_build_name( 'T','A','0','2' );
  status = rtems_task_create(
    Task_name[1],
    28,               /* High priority task */
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[1]
  );
  directive_failed( status, "rtems_task_create of TA01" );

  benchmark_timer_initialize();
  for ( count = 0; count < BENCHMARKS - 1; count++ ) {
    /* message send/recieve */
  }
  tloop_overhead = benchmark_timer_read();

  status = rtems_task_start( Task_id[0], Task01, 0 );
  directive_failed( status, "rtems_task_start of TA01" );

  rtems_task_exit();
}

rtems_task Task01( rtems_task_argument ignored )
{
  rtems_status_code status;

  /* Put a message in the queue so recieve overhead can be found. */
  (void) rtems_message_queue_send( Queue_id, Buffer, MESSAGE_SIZE );

  /* Start up second task, get preempted */
  status = rtems_task_start( Task_id[1], Task02, 0 );
  directive_failed( status, "rtems_task_start" );

  for ( ; count < BENCHMARKS; count++ ) {
    (void) rtems_message_queue_send( Queue_id, Buffer, MESSAGE_SIZE );
  }

  /* Should never reach here */
  rtems_test_assert( false );

}

rtems_task Task02( rtems_task_argument ignored )
{
  size_t size;

  /* find recieve overhead - no preempt or task switch */
  benchmark_timer_initialize();
  (void) rtems_message_queue_receive(
             Queue_id,
             (long (*)[4]) Buffer,
             &size,
             RTEMS_DEFAULT_OPTIONS,
             RTEMS_NO_TIMEOUT
           );
  treceive_overhead = benchmark_timer_read();

  /* Benchmark code */
  benchmark_timer_initialize();
  for ( count = 0; count < BENCHMARKS - 1; count++ ) {
    (void) rtems_message_queue_receive(
             Queue_id,
             (long (*)[4]) Buffer,
             &size,
             RTEMS_DEFAULT_OPTIONS,
             RTEMS_NO_TIMEOUT
           );
  }
  telapsed = benchmark_timer_read();

  put_time(
   "Rhealstone: Intertask Message Latency",
   telapsed,                     /* Total time of all benchmarks */
   BENCHMARKS - 1,               /* Total benchmarks */
   tloop_overhead,               /* Overhead of loops */
   treceive_overhead             /* Overhead of recieve call and task switch */
  );

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              3
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     1
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(1, MESSAGE_SIZE)
#define CONFIGURE_TICKS_PER_TIMESLICE        0
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
