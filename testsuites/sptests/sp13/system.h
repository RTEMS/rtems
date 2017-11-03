/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

void Fill_buffer(
  char  source[],
  long *buffer
);

void Put_buffer(
  void *buffer
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

rtems_task Task_3(
  rtems_task_argument argument
);

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

TEST_EXTERN rtems_id   Queue_id[ 4 ];        /* array of queue ids */
TEST_EXTERN rtems_name Queue_name[ 4 ];      /* array of queue names */

/* test configuration */

#define MESSAGE_SIZE (sizeof(long) * 4)  /* must be multiple of sizeof(long) */

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               4
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     10
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

/*
 *  Created in init.c:
 *    Q1 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, MESSAGE_SIZE ) (1600)
 *    Q2 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 10, MESSAGE_SIZE )   (160)
 *    Q3 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, MESSAGE_SIZE ) (1600)
 *
 *  Q1 and Q2 deleted in task1.c.
 *
 *  Q1 recreated in task1.c:
 *    Q1 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, 20 )           (2000)
 *
 *  Q1 deleted again in task1.c.
 *
 *  Q1 repeatedly created and deleted for 2 messages of 1-1030 bytes
 *  in length. Account for peak usage:
 *    Q1 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 2, 1030 )           (2060)
 *
 *  Because each message requires memory for the message plus two
 *  pointers to place the buffer on lists, it is easier to just
 *  allocate memory for all the message queues. But we can safely
 *  ignore Q2 and the last instance of Q1 since enough memory is
 *  free when the third instance of Q1 is created.
 */
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
   /* Q1 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, MESSAGE_SIZE ) + \
   /* Q2 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 10, MESSAGE_SIZE ) + \
   /* Q3 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, MESSAGE_SIZE ) + \
   /* Q1 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, 20 ) + \
   /* Q1 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 2,  1030 )

#define CONFIGURE_EXTRA_TASK_STACKS         (3 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_DISABLE_SMP_CONFIGURATION

#include <rtems/confdefs.h>

/* end of include file */
