/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#include <tmacros.h>

const char rtems_test_name[] = "SPSTKALLOC";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define MAXIMUM_STACKS 2

typedef struct {
  uint8_t Space[CPU_STACK_MINIMUM_SIZE];
} StackMemory_t;

int            stackToAlloc = 0;
StackMemory_t  Stacks[MAXIMUM_STACKS];
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

  TEST_BEGIN();

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

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_TASK_STACK_ALLOCATOR    StackAllocator
#define CONFIGURE_TASK_STACK_DEALLOCATOR  StackDeallocator

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
