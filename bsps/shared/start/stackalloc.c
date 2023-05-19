/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup bsp_stack
 *
 * @brief Stack initialization, allocation and free functions.
 */

/*
 * Copyright (C) 2009, 2013 embedded brains GmbH & Co. KG
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

#include <bsp/stackalloc.h>

#include <rtems.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/wkspace.h>

#include <bsp/linker-symbols.h>

static Heap_Control bsp_stack_heap;

void bsp_stack_allocate_init(size_t stack_space_size)
{
  _Heap_Initialize(
    &bsp_stack_heap,
    bsp_section_stack_begin,
    (uintptr_t) bsp_section_stack_size,
    CPU_STACK_ALIGNMENT
  );
}

void *bsp_stack_allocate(size_t size)
{
  void *stack = NULL;

  if (bsp_stack_heap.area_begin != 0) {
    stack = _Heap_Allocate(&bsp_stack_heap, size);
  }

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
