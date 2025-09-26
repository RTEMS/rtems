/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Test to trip reordering of threads on thread queues when
 * their priority changes.
 */

/*
 * Copyright (C) 2007. On-Line Applications Research Corporation (OAR).
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
#include <stdio.h>

/********************************************************************/
/* define this to use the RTEMS 4.5 scheme for object names */
/* #define TEST_ON_RTEMS_45 */

/* define this to print the Id of the calling task */
/* #define TEST_ON_TASK_ID */

/********************************************************************/

#define CONFIGURE_INIT

#include <bsp.h>
#include <stdio.h>
#include "tmacros.h"

#include <rtems/score/threadimpl.h>

const char rtems_test_name[] = "SP 34";

rtems_task BlockingTasks(rtems_task_argument arg);
rtems_task Init(rtems_task_argument ignored);
const char *CallerName(void);

/*
 *  CallerName -- print the calling tasks name or id as configured
 */
const char *CallerName(void)
{
  static char buffer[32];
  Thread_Control *executing = _Thread_Get_executing();
#if defined(TEST_PRINT_TASK_ID)
  sprintf( buffer, "0x%08x -- %d",
      rtems_task_self(), _Thread_Get_priority( executing ) );
#else
  volatile union {
    uint32_t u;
    unsigned char c[4];
  } TempName;

  #if defined(TEST_ON_RTEMS_45)
    TempName.u = *(uint32_t *)executing->Object.name;
  #else
    TempName.u = executing->Object.name.name_u32;
  #endif
    sprintf( buffer, "%c%c%c%c -- %" PRIdPriority_Control,
      TempName.c[0], TempName.c[1], TempName.c[2], TempName.c[3],
      _Thread_Get_priority( executing )
  );
#endif
  return buffer;
}

#define NUMBER_OF_BLOCKING_TASKS 5

/* RTEMS ids of blocking threads */
rtems_id  Blockers[NUMBER_OF_BLOCKING_TASKS];

/* Semaphore they are all blocked on */
rtems_id  Semaphore;

rtems_task BlockingTasks(rtems_task_argument arg)
{
  rtems_status_code   status;
  rtems_task_priority pri = (rtems_task_priority) arg;
  rtems_task_priority opri;
  rtems_task_priority npri;

  status = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &opri);
  directive_failed( status, "rtems_task_set_priority" );

  printf(
    "semaphore_obtain -- BlockingTask %" PRIdrtems_task_priority
      " @ pri=%" PRIdrtems_task_priority ") blocks\n",
    pri,
    opri
  );
  status = rtems_semaphore_obtain(Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  directive_failed( status, "rtems_semaphore_obtain" );

  /* priority should have changed while blocked */
  status = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &npri);
  directive_failed( status, "rtems_task_set_priority" );

  printf(
    "semaphore_obtain -- BlockingTask %" PRIdrtems_task_priority
      " @ pri=%" PRIdrtems_task_priority ") returns\n",
    pri,
    npri
  );

  rtems_task_exit();
}

/*************************************************************************/
/**********************        INITIALIZATION        *********************/
/*************************************************************************/

rtems_task Init(rtems_task_argument ignored)
{
  (void) ignored;

  rtems_status_code   status;
  int                 i;

  TEST_BEGIN();

  /* Create synchronisation semaphore for LocalHwIsr -> Test Tasks */
  status = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),           /* name */
    0,                                               /* initial count = 0 */
    RTEMS_LOCAL              |
    RTEMS_COUNTING_SEMAPHORE |
    RTEMS_PRIORITY,
    0,
    &Semaphore);                                    /* *id */
  directive_failed( status, "rtems_semaphore_create" );

  /* Create and start all tasks in the test */

  for (i = 0; i < NUMBER_OF_BLOCKING_TASKS; i++) {
    status = rtems_task_create(
      rtems_build_name('B','L','K','0'+i),               /* Name */
      (rtems_task_priority) 2+i,                         /* Priority */
      RTEMS_MINIMUM_STACK_SIZE*2,                        /* Stack size (8KB) */
      RTEMS_DEFAULT_MODES | RTEMS_NO_ASR,                /* Mode */
      RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,   /* Attributes */
      &Blockers[i]);                                     /* Assigned ID */
    directive_failed( status, "rtems_task_create (BLKn)" );

    printf( "Blockers[%d] Id = 0x%08" PRIxrtems_id "\n", i, Blockers[i] );
    status = rtems_task_start(
      Blockers[i],
      BlockingTasks,
      (rtems_task_argument)i
    );
    directive_failed( status, "rtems_task_start (BLKn)" );
  }

  status = rtems_task_wake_after( 100 );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "rtems_task_set_priority -- invert priorities of tasks" );
  for (i = 0; i < NUMBER_OF_BLOCKING_TASKS; i++) {
    rtems_task_priority opri;
    rtems_task_priority npri;

    npri = (rtems_task_priority) (2 + NUMBER_OF_BLOCKING_TASKS - i - 1);

    status = rtems_task_set_priority(Blockers[i], npri, &opri);
    directive_failed( status, "rtems_task_set_priority" );
  }

  for (i = 0; i < NUMBER_OF_BLOCKING_TASKS; i++) {
    puts( "rtems_semaphore_release -- OK" );
    status = rtems_semaphore_release(Semaphore);
    directive_failed( status, "rtems_semaphore_release" );

    status = rtems_task_wake_after( 100 );
    directive_failed( status, "rtems_task_wake_after" );
  }

  /* exit the test */
  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS \
          (RTEMS_MINIMUM_STACK_SIZE * NUMBER_OF_BLOCKING_TASKS)

#define CONFIGURE_MAXIMUM_TASKS 6
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#include <rtems/confdefs.h>

/* end of file */
