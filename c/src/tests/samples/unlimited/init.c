/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT

#include "system.h"
#include <stdio.h>
#include <stdlib.h>

rtems_id task_id[MAX_TASKS];

void test1();
void test2();

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_task_priority old_priority;
  rtems_mode          old_mode;
  rtems_unsigned32    task;

  /* lower the task priority to allow created tasks to execute */
  
  rtems_task_set_priority(RTEMS_SELF, 20, &old_priority);
  rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);
  
  printf( "\n*** UNLIMITED TASK TEST ***\n" );

  /*
   * Invalid state if the task id is 0
   */
    
  for (task = 0; task < MAX_TASKS; task++)
    task_id[task] = 0;

  test1();
  test2();
  test3();
  
  printf( "\n*** END OF UNLIMITED TASK TEST ***\n" );
  exit( 0 );
}

rtems_task test_task(
  rtems_task_argument my_number
  )
{
  rtems_event_set out;
  
  printf( "task %i has started.\n",  my_number);

  rtems_event_receive(1, RTEMS_WAIT | RTEMS_EVENT_ANY, 0, &out);
  
  printf( "task %i ending.\n",  my_number);

  rtems_task_delete(RTEMS_SELF);
}

void destory_all_tasks(
  const char *who
)
{
  rtems_unsigned32 task;
  
  /*
   *  If the id is not zero, signal the task to delete.
   */
  
  for (task = 0; task < MAX_TASKS; task++)
    if (task_id[task])
    {
      printf(" %s : signal task %08x to delete, ", who, task_id[task]);
      fflush(stdout);
      rtems_event_send(task_id[task], 1);
      task_id[task] = 0;
    }
}

boolean status_code_bad(
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





