/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <bsp.h>

#include <rtems/test.h>
#include <rtems/score/heapimpl.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPHEAPPROT";

#ifdef HEAP_PROTECTION
  static void test_heap_block_error(Heap_Control *heap, Heap_Block *block)
  {
    bool *error = heap->Protection.handler_data;

    *error = true;
  }

  static void test_heap_initialize(
    Heap_Control *heap,
    void *begin,
    uintptr_t size,
    bool *error
  )
  {
    size = _Heap_Initialize(heap, begin, size, 0);
    assert(size > 0);

    heap->Protection.handler_data = error;
    heap->Protection.block_error = test_heap_block_error;

    *error = false;
  }

  static void test_heap_protection(void)
  {
    Heap_Control heap;
    Heap_Block *block = NULL;
    char area [512];
    uintptr_t *p = NULL;
    uintptr_t max_size = 0;
    bool ok = false;
    bool error = false;

    /* Test double free */

    test_heap_initialize(&heap, area, sizeof(area), &error);

    max_size = heap.stats.free_size
      - HEAP_BLOCK_HEADER_SIZE + HEAP_ALLOC_BONUS;

    p = _Heap_Allocate(&heap, max_size);
    assert(p != NULL);

    ok = _Heap_Free(&heap, p);
    assert(ok && !error);

    ok = _Heap_Free(&heap, p);
    assert(ok && error);

    /* Test begin overwrite */

    test_heap_initialize(&heap, area, sizeof(area), &error);

    p = _Heap_Allocate(&heap, max_size);
    assert(p != NULL);

    *(p - 1) = 0;

    ok = _Heap_Free(&heap, p);
    assert(ok && error);

    /* Test end overwrite */

    test_heap_initialize(&heap, area, sizeof(area), &error);

    p = _Heap_Allocate(&heap, max_size);
    assert(p != NULL);

    *(uintptr_t *)((char *) p + max_size) = 0;

    ok = _Heap_Free(&heap, p);
    assert(ok && error);

    /* Test use after free */

    test_heap_initialize(&heap, area, sizeof(area), &error);

    p = _Heap_Allocate(&heap, max_size);
    assert(p != NULL);

    ok = _Heap_Free(&heap, p);
    assert(ok && !error);

    *p = 0;

    block = _Heap_Block_of_alloc_area((uintptr_t) p, heap.page_size);
    block->Protection_begin.next_delayed_free_block = HEAP_PROTECTION_OBOLUS;
    ok = _Heap_Free(&heap, p);
    assert(ok && error);
  }
#else
  #define test_heap_protection() ((void) 0)
#endif

static rtems_task Init(rtems_task_argument argument)
{
  TEST_BEGIN();

  test_heap_protection();

  TEST_END();

  exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
