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

void test2()
{
  rtems_status_code   result;
  rtems_unsigned32    remove_task;
  rtems_unsigned32    task;
  rtems_unsigned32    block;
  rtems_unsigned32    task_count = 0;
  rtems_id            removed_ids[TASK_ALLOCATION_SIZE * 2];
  
  char               c1 = 'a';
  char               c2 = 'a';
  char               c3 = '0';
  char               c4 = '0';
  
  printf( "\n TEST2 : re-allocate of index numbers, and a block free'ed and one inactive\n" );

  /*
   *  Allocate enought tasks so the Inactive list is empty. Remember
   *  to count the Init task, ie ... - 1.
   */
  
  while (task_count < ((TASK_ALLOCATION_SIZE * 5) - TASK_INDEX_OFFSET))
  {
    rtems_name name;

    printf(" TEST2 : creating task '%c%c%c%c', ", c1, c2, c3, c4);
    
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
   *  Take out the second and fourth allocation size block of tasks
   */

  if (task_count != ((TASK_ALLOCATION_SIZE * 5) - TASK_INDEX_OFFSET)) {
    printf( " FAIL2 : not enough tasks created -\n"
            "         task created = %i, required number = %i\n",
            task_count, (TASK_ALLOCATION_SIZE * 5) - TASK_INDEX_OFFSET);
    destory_all_tasks("TEST2");
    exit( 1 );
  }

  task = 0;
  
  for (block = 1; block < 4; block += 2)
  {
    for (remove_task = (block * TASK_ALLOCATION_SIZE) - TASK_INDEX_OFFSET;
         remove_task < (((block + 1) * TASK_ALLOCATION_SIZE) - TASK_INDEX_OFFSET);
         remove_task++)
    {
      if (!task_id[remove_task])
      {
        printf( " FAIL2 : remove task has a 0 id -\n"
                "         task number = %i\n",
                remove_task);
        destory_all_tasks("TEST2");
        exit( 1 );
      }

      /*
       * Save the id's to match them against the reallocated ids
       */
      
      removed_ids[task++] = task_id[remove_task];
      
      printf(" TEST2 : block %i remove, signal task %08x, ", block, task_id[remove_task]);
      rtems_event_send(task_id[remove_task], 1);
      task_id[remove_task] = 0;
    }
  }

  for (task = 0; task < (TASK_ALLOCATION_SIZE * 2); task++)
  {
    rtems_name       name;
    rtems_unsigned32 id_slot;

    /*
     *  Find a free slot in the task id table.
     */

    for (id_slot = 0; id_slot < MAX_TASKS; id_slot++)
      if (!task_id[id_slot])
        break;
     
    if (id_slot == MAX_TASKS)
    {
      printf( " FAIL2 : no free task id slot.\n");
      destory_all_tasks("TEST2");
      exit( 1 );
    }

    printf(" TEST2 : creating task '%c%c%c%c', ", c1, c2, c3, c4);
    
    name = rtems_build_name(c1, c2, c3, c4);

    result = rtems_task_create(name,
                               10,
                               RTEMS_MINIMUM_STACK_SIZE,
                               RTEMS_DEFAULT_ATTRIBUTES,
                               RTEMS_LOCAL,
                               &task_id[id_slot]);
    
    if (status_code_bad(result))
    {
      printf( " FAIL2 : re-creating a task -\n"
              "         task number = %i\n",
              id_slot);
      destory_all_tasks("TEST2");
      exit( 1 );
    }
    
    printf("number = %3i, id = %08x, starting, ", task_count, task_id[id_slot]);
    
    result = rtems_task_start(task_id[id_slot],
                              test_task,
                              (rtems_task_argument) task_count);
    
    if (status_code_bad(result))
    {
      printf( " FAIL : re-starting a task -\n"
              "        task number = %i\n",
              id_slot);
      destory_all_tasks("TEST2");
      exit( 1 );
    }
    
    /*
     *  Update the name.
     */
    
    NEXT_TASK_NAME(c1, c2, c3, c4);
    
    /*
     *  Search the removed ids to see if it existed, clear the removed id when found
     */
    
    for (remove_task = 0; remove_task < (TASK_ALLOCATION_SIZE * 2); remove_task++)
      if (removed_ids[remove_task] == task_id[id_slot])
      {
        removed_ids[remove_task] = 0;
        break;
      }

    /*
     *  If not located in the removed id table, check and make sure it is not
     *  already allocated
     */
    
    if (remove_task == (TASK_ALLOCATION_SIZE * 2))
    {
      rtems_unsigned32 allocated_id;
      
      for (allocated_id = 0; allocated_id < MAX_TASKS; allocated_id++)
        if ((task_id[id_slot] == task_id[allocated_id]) && (id_slot != allocated_id))
        {
          printf( " FAIL2 : the new id is the same as an id already allocated -\n"
                  "         task id = %08x\n",
                  task_id[id_slot]);
          exit( 1 );
        }
      
      printf( " FAIL2 : could not find the task id in the removed table -\n"
              "         task id = %08x\n",
              task_id[id_slot]);
      exit( 1 );
    }

    task_count++;
  }
  
  destory_all_tasks("TEST2");
  
  printf( " TEST2 : completed\n" );
}

