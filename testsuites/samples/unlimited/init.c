/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
