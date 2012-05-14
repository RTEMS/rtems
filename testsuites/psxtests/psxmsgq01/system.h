/**
 *  @file
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* functions */

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <tmacros.h>

void *POSIX_Init(void *argument);
void *Task_1_through_3(void *argument);
void *Task_4(void *argument);
void *Task_5(void *argument);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS         5
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 10
#define CONFIGURE_MAXIMUM_POSIX_TIMERS          4
#define CONFIGURE_MAXIMUM_TIMERS                4

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
        (RTEMS_MINIMUM_STACK_SIZE * 10)

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN pthread_t        Init_id;
TEST_EXTERN pthread_t        Task_id;

#define MSGSIZE   9
#define MAXMSG    4

typedef enum {
  RD_QUEUE,   /* Read only queue               */
  WR_QUEUE,   /* Write only queue              */
  RW_QUEUE,   /* Read Write non-blocking queue */
  BLOCKING,   /* Read Write blocking queue     */
  DEFAULT_RW, /* default must be last          */
  CLOSED,     /* Created and closed queue      */
  NUMBER_OF_TEST_QUEUES
} Test_Queue_Types;
extern int Priority_Order[MAXMSG+1];
void *Task_1 ( void *argument );
void *Task_2( void *argument );
void *Task_3( void *argument );
void *Task_4( void *argument );

/* end of include file */
