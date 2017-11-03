/**
 * @file
 *
 * @ingroup sptests
 *
 * @brief Test that the task stack allocator works.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <tmacros.h>

const char rtems_test_name[] = "SPSTKALLOC 2";

#include <stdio.h>
#include <inttypes.h>

#include <rtems/score/heapimpl.h>

#define TASK_COUNT 5

#define STACK_HEAP_PAGE_SIZE 1024

static Heap_Control task_stack_heap;

static void task_stack_init(size_t stack_space_size);

static void *task_stack_allocate(size_t stack_size);

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

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS TASK_COUNT

#define CONFIGURE_TASK_STACK_ALLOCATOR_INIT task_stack_init
#define CONFIGURE_TASK_STACK_ALLOCATOR task_stack_allocate
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

static void task_stack_free(void *addr)
{
  _Heap_Free(&task_stack_heap, addr);
}
