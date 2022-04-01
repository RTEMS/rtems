/* SPDX-License-Identifier: BSD-2-Clause */

/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <tmacros.h>

/* functions */

rtems_timer_service_routine Delayed_send_event(
  rtems_id  timer_id,
  void     *id_ptr
);

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_task Delayed_events_task(
  rtems_task_argument argument
);

rtems_task Message_queue_task(
  rtems_task_argument index
);

rtems_task Partition_task(
  rtems_task_argument argument
);

rtems_task Semaphore_task(
  rtems_task_argument argument
);

void Exit_test( void );

/* configuration information */

#define CONFIGURE_MP_APPLICATION

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_TICKS_PER_TIMESLICE   1
#define CONFIGURE_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(1)

#define CONFIGURE_MAXIMUM_TASKS               5
#if ( CONFIGURE_MP_NODE_NUMBER == 1 )
#define CONFIGURE_MAXIMUM_TIMERS              12
#define CONFIGURE_MAXIMUM_SEMAPHORES          1
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      1
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 1, 16 )
#define CONFIGURE_MAXIMUM_PARTITIONS          1
#elif ( CONFIGURE_MP_NODE_NUMBER == 2 )
#define CONFIGURE_MAXIMUM_TIMERS              2
#endif

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* variables */

TEST_EXTERN rtems_id   Event_task_id[ 2 ];       /* event task ids */

TEST_EXTERN rtems_id   Semaphore_task_id[ 3 ];   /* semaphore task ids */
TEST_EXTERN rtems_name Semaphore_task_name[ 3 ]; /* semaphore task names */

TEST_EXTERN rtems_id   Queue_task_id[ 3 ];       /* message queue task ids */
TEST_EXTERN rtems_name Queue_task_name[ 3 ];     /* message queue task names */

TEST_EXTERN rtems_id   Partition_task_id[ 4 ];   /* partition task ids */
TEST_EXTERN rtems_name Partition_task_name[ 4 ]; /* partition task names */

TEST_EXTERN rtems_id   Partition_id[ 3 ];    /* partition ids */
TEST_EXTERN rtems_name Partition_name[ 3 ];  /* partition names */

TEST_EXTERN rtems_id   Semaphore_id[ 3 ];    /* semaphore ids */
TEST_EXTERN rtems_name Semaphore_name[ 3 ];  /* semaphore names */

TEST_EXTERN rtems_id   Queue_id[ 3 ];        /* message queue ids */
TEST_EXTERN rtems_name Queue_name[ 3 ];      /* message queue names */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* remote event task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* remote event task names */

TEST_EXTERN rtems_id   Timer_id[ 4 ];         /* event timer ids */
TEST_EXTERN rtems_name Timer_name[ 4 ];       /* event timer names */

TEST_EXTERN uint32_t   Msg_buffer[ 4 ][ 4 ];

TEST_EXTERN volatile bool Stop_Test;
TEST_EXTERN rtems_id timer_id;

#define EVENT_TASK_DOT_COUNT        100
#define EVENT_SEND_DOT_COUNT        100
#define DELAYED_EVENT_DOT_COUNT     1000
#define MESSAGE_DOT_COUNT           200
#define PARTITION_DOT_COUNT         200
#define SEMAPHORE_DOT_COUNT         200

/* end of include file */
