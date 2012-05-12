/*
 *  Test for rtems_semaphore_flush
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <tmacros.h>

#include <stdio.h>
#include <stdlib.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void starttask(int arg);
rtems_task subtask(rtems_task_argument arg);
void doTest(void);

#define NTASK 4

#if defined(USE_COUNTING_SEMAPHORE)
  #define TEST_NAME                 "27a"
  #define TEST_SEMAPHORE_TYPE       "counting"
  #define TEST_SEMAPHORE_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#else
  #define TEST_NAME                 "27"
  #define TEST_SEMAPHORE_TYPE       "binary"
  #define TEST_SEMAPHORE_ATTRIBUTES (RTEMS_LOCAL| \
	    RTEMS_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY| \
	    RTEMS_NO_PRIORITY_CEILING|RTEMS_FIFO)
#endif

rtems_id semaphore;
volatile int flags[NTASK];

rtems_task subtask(
  rtems_task_argument arg
)
{
  rtems_status_code sc;

  for (;;) {
    flags[arg]++;
    sc = rtems_semaphore_obtain(semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (sc == RTEMS_SUCCESSFUL)
      puts("Obtained semaphore -- and should not have done so!");
    else if (sc != RTEMS_UNSATISFIED)
      printf("Can't get semaphore: %s\n", rtems_status_text(sc));
  }
}

void starttask(
  int arg
)
{
  rtems_id tid;
  rtems_status_code sc;
  rtems_task_priority priority;

  rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
  sc = rtems_task_create(rtems_build_name('S', 'R', 'V', arg + 'A'),
    priority,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
    RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
    &tid);
  directive_failed( sc, "task create" );

  sc = rtems_task_start(tid, subtask, (rtems_task_argument) arg);
  directive_failed( sc, "task start" );
}

void doTest(void)
{
  rtems_status_code sc;
  int               pass, i;

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'F'),
    0,
    TEST_SEMAPHORE_ATTRIBUTES,
    0,
    &semaphore);
  directive_failed( sc, "semaphore create" );

  for (i = 0 ; i < NTASK ; i++)
    flags[i] = 0;

  for (i = 0 ; i < NTASK ; i++)
    starttask(i);

  for (pass = 1 ; pass < 10 ; pass++) {
    rtems_task_wake_after(1);
    for (i = 0 ; i < NTASK ; i++) {
      if (flags[i] != pass)
        printf("flags[%d] = %d -- expected %d\n", i, flags[i], pass);
    }
    sc = rtems_semaphore_flush(semaphore);
    directive_failed( sc, "semaphore flush" );
  }

  printf("Flushed all waiting tasks\n" );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "\n\n*** TEST " TEST_NAME " ***" );
  puts( "Testing " TEST_SEMAPHORE_TYPE " semaphore flush" );
  doTest();
  puts( "*** END OF TEST " TEST_NAME " ***" );

  rtems_test_exit(0);
}

/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               6
#define CONFIGURE_MAXIMUM_SEMAPHORES          1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/

