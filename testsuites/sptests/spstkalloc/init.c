/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define MAXIMUM_STACKS 3

typedef struct {
  uint8_t Space[CPU_STACK_MINIMUM_SIZE];
} StackMemory_t;

int            stackToAlloc = 0;
StackMemory_t  Stacks[3];
void          *StackDeallocated = NULL;

static void *StackAllocator(size_t size)
{
  if (stackToAlloc < MAXIMUM_STACKS)
    return &Stacks[stackToAlloc++];
  return NULL;
}

static void StackDeallocator(void *stack)
{
  StackDeallocated = stack;
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code rc;
  rtems_id          taskId;
  rtems_id          taskId1;

  puts( "\n\n*** TEST OF STACK ALLOCATOR PLUGIN ***" );

  puts( "Init - create task TA1 to use custom stack allocator - OK" );
  rc = rtems_task_create(
     rtems_build_name( 'T', 'A', '1', ' ' ),
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &taskId
  );
  directive_failed( rc, "rtems_task_create of TA1" );

  puts( "Init - create task TA1 to have custom stack allocator fail" );
  rc = rtems_task_create(
     rtems_build_name( 'F', 'A', 'I', 'L' ),
     1,
     RTEMS_MINIMUM_STACK_SIZE,
     RTEMS_DEFAULT_MODES,
     RTEMS_DEFAULT_ATTRIBUTES,
     &taskId1
  );
  fatal_directive_status( rc, RTEMS_UNSATISFIED, "rtems_task_create of FAIL" );

  puts( "Init - delete task TA1 to use custom stack deallocator - OK" );
  rc = rtems_task_delete( taskId );
  directive_failed( rc, "rtems_task_delete of TA1" );

  puts( "*** END OF OF STACK ALLOCATOR PLUGIN TEST ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_TASK_STACK_ALLOCATOR    StackAllocator
#define CONFIGURE_TASK_STACK_DEALLOCATOR  StackDeallocator

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
