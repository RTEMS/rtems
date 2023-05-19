/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <bsp.h>

#include <rtems/test-info.h>
#include <rtems/score/heapimpl.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPHEAPPROT";

#ifdef HEAP_PROTECTION
  static void test_heap_block_error(
     Heap_Control *heap,
     Heap_Block *block,
     Heap_Error_reason reason
  )
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
