/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <rtems/score/stack.h>
#include <rtems/score/thread.h>
#include <rtems/score/assert.h>

/**
 * @brief Default stack allocator allocate for IDLE threads.
 *
 * The default allocator for IDLE thread stacks gets the memory from a
 * statically allocated area provided via confdefs.h.
 *
 * @param cpu is the index of the CPU for the IDLE thread using this stack.
 *
 * @param stack_size[in] is pointer to a size_t object.  On function
 *   entry, the object contains the size of the stack area to allocate in
 *   bytes.
 *
 * @return Returns the pointer to begin of the allocated stack area.
 */
static void *_Stack_Allocator_allocate_for_idle_default(
  uint32_t  cpu_index,
  size_t   *stack_size
)
{
#if defined(RTEMS_SMP)
  return &_Thread_Idle_stacks[ cpu_index * ( *stack_size ) ];
#else
  _Assert( cpu_index == 0 );
  return &_Thread_Idle_stacks[ 0 ];
#endif
}

const Stack_Allocator_allocate_for_idle _Stack_Allocator_allocate_for_idle =
        _Stack_Allocator_allocate_for_idle_default;
