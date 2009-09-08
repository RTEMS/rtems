/**
 * @file
 *
 * @ingroup bsp_shared
 *
 * @brief Stack initialization, allocation and free functions.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/score/wkspace.h>

#include <bsp/stackalloc.h>

#define BSP_STACK_MAGIC 0xdeadbeef

static Heap_Control bsp_stack_heap = {
  .page_size = BSP_STACK_MAGIC
};

void bsp_stack_initialize(void *begin, uintptr_t size)
{
  bsp_stack_heap.area_begin = (uintptr_t) begin;
  bsp_stack_heap.area_end = size;
}

void *bsp_stack_allocate(size_t size)
{
  void *stack = NULL;

  if (bsp_stack_heap.page_size == BSP_STACK_MAGIC) {
    uintptr_t rv = _Heap_Initialize(
      &bsp_stack_heap,
      (void *) bsp_stack_heap.area_begin,
      bsp_stack_heap.area_end,
      CPU_STACK_ALIGNMENT
    );
    if (rv == 0) {
      return NULL;
    }
  }

  stack = _Heap_Allocate(&bsp_stack_heap, size);

  if (stack == NULL) {
    stack = _Workspace_Allocate(size);
  }

  return stack;
}

void bsp_stack_free(void *stack)
{
  bool ok = _Heap_Free(&bsp_stack_heap, stack);

  if (!ok) {
    _Workspace_Free(stack);
  }
}
