/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup sptests
 *
 * @brief Test that the task stack allocator works.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

const char rtems_test_name[] = "SPSTKALLOC 2";

#include <stdio.h>
#include <inttypes.h>

#include <rtems/malloc.h>
#include <rtems/score/heapimpl.h>

#define TASK_COUNT 5

#define STACK_HEAP_PAGE_SIZE 1024

static Heap_Control task_stack_heap;

static void task_stack_init(size_t stack_space_size);

static void *task_stack_allocate(size_t stack_size);

static void *task_stack_allocate_for_idle(uint32_t unused, size_t *stack_size);

static void task_stack_free(void *addr);

static void print_info(void)
{
  Heap_Information_block info;

  _Heap_Get_information(&task_stack_heap, &info);

  printf(
    "used blocks = %" PRIuPTR ", "
    "largest used block = %" PRIuPTR ", "
    "used space = %" PRIuPTR "\n"
    "free blocks = %" PRIuPTR ", "
    "largest free block = %" PRIuPTR ", "
    "free space = %" PRIuPTR "\n",
    info.Used.number,
    info.Used.largest,
    info.Used.total,
    info.Free.number,
    info.Free.largest,
    info.Free.total
  );
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_name name = rtems_build_name('S', 'T', 'K', 'A');
  rtems_id id = RTEMS_ID_NONE;
  int i = 0;

  TEST_BEGIN();

  print_info();

  for (i = 2; i < TASK_COUNT; ++i) {
    sc = rtems_task_create(
      name,
      RTEMS_MINIMUM_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed(sc, "rtems_task_create");
  }

  sc = rtems_task_create(
    name,
    RTEMS_MINIMUM_PRIORITY,
    2 * RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  fatal_directive_status(sc, RTEMS_UNSATISFIED, "rtems_task_create");

  sc = rtems_task_create(
    name,
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed(sc, "rtems_task_create");

  print_info();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/*
 * Avoid a dependency on errno which might be a thread-local object.  This test
 * assumes that no thread-local storage object is present.
 */
#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

/*
 * This test requires full control over the present thread-local objects.  This
 * is necessary for the custom stack allocator below.  In certain Newlib
 * configurations, the Newlib reentrancy support may add thread-local objects.
 */
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_MAXIMUM_TASKS TASK_COUNT

#define CONFIGURE_TASK_STACK_ALLOCATOR_INIT task_stack_init
#define CONFIGURE_TASK_STACK_ALLOCATOR task_stack_allocate
#define CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE task_stack_allocate_for_idle
#define CONFIGURE_TASK_STACK_DEALLOCATOR task_stack_free
#define CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE
#define CONFIGURE_TASK_STACK_FROM_ALLOCATOR(stack_size) \
  ((stack_size) + HEAP_BLOCK_HEADER_SIZE + STACK_HEAP_PAGE_SIZE - 1)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

static char task_stack_space
  [CONFIGURE_TASK_STACK_FROM_ALLOCATOR(_CONFIGURE_STACK_SPACE_SIZE)];

static void task_stack_init(size_t stack_space_size)
{
  bool ok = _Heap_Initialize(
    &task_stack_heap,
    task_stack_space,
    sizeof(task_stack_space),
    STACK_HEAP_PAGE_SIZE
  );

  rtems_test_assert(stack_space_size == _CONFIGURE_STACK_SPACE_SIZE);
  rtems_test_assert(ok);
}

static void *task_stack_allocate(size_t stack_size)
{
  return _Heap_Allocate(&task_stack_heap, stack_size);
}

static void *task_stack_allocate_for_idle(uint32_t unused, size_t *stack_size)
{
  return rtems_heap_allocate_aligned_with_boundary(
    *stack_size,
    CPU_STACK_ALIGNMENT,
    0
  );
}

static void task_stack_free(void *addr)
{
  _Heap_Free(&task_stack_heap, addr);
}
