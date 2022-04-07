/* SPDX-License-Identifier: BSD-2-Clause */

/*
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

#define CONFIGURE_INIT

#include "system.h"
#include "tmacros.h"
#include <stdio.h>
#include <stdlib.h>

const char rtems_test_name[] = "UNLIMITED TASK";

rtems_id task_id[MAX_TASKS];

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_task_priority old_priority;
  rtems_mode          old_mode;
  uint32_t            task;

  TEST_BEGIN();

  /* lower the task priority to allow created tasks to execute */

  rtems_task_set_priority(
    RTEMS_SELF, RTEMS_MAXIMUM_PRIORITY - 1, &old_priority);
  rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);

  /*
   * Invalid state if the task id is 0
   */

  for (task = 0; task < MAX_TASKS; task++)
    task_id[task] = 0;

  test1();
  test2();
  test3();

  TEST_END();
  exit( 0 );
}

rtems_task test_task(
  rtems_task_argument my_number
)
{
  rtems_event_set out;
  unsigned int    my_n = (unsigned int) my_number;

  printf( "task %u has started.\n",  my_n);

  rtems_event_receive(1, RTEMS_WAIT | RTEMS_EVENT_ANY, 0, &out);

  printf( "task %u ending.\n",  my_n);

  rtems_task_exit();
}

void destroy_all_tasks(
  const char *who
)
{
  uint32_t   task;

  /*
   *  If the id is not zero, signal the task to delete.
   */

  for (task = 0; task < MAX_TASKS; task++) {
    if (task_id[task]) {
      printf(
        " %s : signal task %08" PRIxrtems_id " to delete, ",
         who,
         task_id[task]
      );
      fflush(stdout);
      rtems_event_send(task_id[task], 1);
      task_id[task] = 0;
    }
  }
}

bool status_code_bad(
  rtems_status_code status_code
)
{
  if (status_code != RTEMS_SUCCESSFUL)
  {
    printf("failure, ");

    if (status_code == RTEMS_TOO_MANY)
    {
      printf("too many.\n");
      return TRUE;
    }
    if (status_code == RTEMS_UNSATISFIED)
    {
      printf("unsatisfied.\n");
      return TRUE;
    }

    printf("error code = %i\n", status_code);
    exit( 1 );
  }
  return FALSE;
}
