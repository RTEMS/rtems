/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Test for rtems_semaphore_flush
 *
 *  COPYRIGHT (c) 1989-2012.
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

const char rtems_test_name[] = "SP " TEST_NAME;

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
  TEST_BEGIN();
  puts( "Testing " TEST_SEMAPHORE_TYPE " semaphore flush" );
  doTest();
  TEST_END();

  rtems_test_exit(0);
}

/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               6
#define CONFIGURE_MAXIMUM_SEMAPHORES          1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/

