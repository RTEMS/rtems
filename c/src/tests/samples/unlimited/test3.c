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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"
#include <stdio.h>

void test3()
{
  rtems_status_code   result;
  rtems_unsigned32    remove_task;
  rtems_unsigned32    block;
  rtems_unsigned32    task_count = 0;
  
  char               c1 = 'a';
  char               c2 = 'a';
  char               c3 = '0';
  char               c4 = '0';
  
  printf( "\n TEST3 : free more than 3 x allocation size, but not the same block,\n"
            "         then free a block\n");

  /*
   *  Check the value of the allocation unit
   */

  if (TASK_ALLOCATION_SIZE < 4)
  {
    printf( " FAIL3 : task allocation size must be greater than 4.\n");
    exit( 1 );
  }
  
   /*
   *  Allocate as many tasks as possible.
   */
  
  while (task_count < MAX_TASKS)
  {
    rtems_name name;

    printf(" TEST3 : creating task '%c%c%c%c', ", c1, c2, c3, c4);
    
    name = rtems_build_name(c1, c2, c3, c4);

    result = rtems_task_create(name,
                               10,
                               RTEMS_MINIMUM_STACK_SIZE,
                               RTEMS_DEFAULT_ATTRIBUTES,
                               RTEMS_LOCAL,
                               &task_id[task_count]);

    if (status_code_bad(result))
      break;
    
    printf("number = %3i, id = %08x, starting, ", task_count, task_id[task_count]);
    fflush(stdout);
    
    result = rtems_task_start(task_id[task_count],
                              test_task,
                              (rtems_task_argument) task_count);
    
    if (status_code_bad(result))
      break;
    
    /*
     *  Update the name.
     */
    
    NEXT_TASK_NAME(c1, c2, c3, c4);
    
    task_count++;
  }

  /*
   *  Take out 3 tasks from each block of allocated tasks. Do this for
   *  allocation size number of blocks.
   */

  if (task_count < (TASK_ALLOCATION_SIZE * 11))
  {
    printf( " FAIL3 : not enough tasks created -\n"
            "         task created = %i, required number = %i\n",
            task_count, (TASK_ALLOCATION_SIZE * 11));
    exit( 1 );
  }

  for (block = 0; block < TASK_ALLOCATION_SIZE; block++)
  {
    for (remove_task = ((block * TASK_ALLOCATION_SIZE) - TASK_INDEX_OFFSET);
         remove_task < (((block * TASK_ALLOCATION_SIZE) + 3) - TASK_INDEX_OFFSET);
         remove_task++)
    {
      if (!task_id[remove_task])
      {
        printf( " FAIL3 : remove task has a 0 id -\n"
                "         task number = %i\n",
                remove_task);
        exit( 1 );
      }

      printf(" TEST3 : remove, signal task %08x, ", task_id[remove_task]);
      rtems_event_send(task_id[remove_task], 1);
      task_id[remove_task] = 0;
    }
  }

  /*
   *  Remove a complete block, not the first, forces a scan of the blocks in the
   *  allocator's free routine
   */
   
  for (remove_task = (TASK_ALLOCATION_SIZE - TASK_INDEX_OFFSET);
       remove_task < ((TASK_ALLOCATION_SIZE * 2) - - TASK_INDEX_OFFSET);
       remove_task++)
  {
    if (task_id[remove_task])
    {
      printf(" TEST3 : remove, signal task %08x, ", task_id[remove_task]);
      rtems_event_send(task_id[remove_task], 1);
      task_id[remove_task] = 0;
    }
  }
  
  destory_all_tasks("TEST3");
  
  printf( " TEST3 : completed\n" );
}
