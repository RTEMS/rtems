/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

#include <rtems/score/memory.h>

void *_Memory_Allocate(
  const Memory_Information *information,
  uintptr_t                 size,
  uintptr_t                 alignment
)
{
  Memory_Area       *area;
  const Memory_Area *end;
  uintptr_t          alignment_mask;

  area = &information->areas[ 0 ];
  end = &information->areas[ information->count ];
  alignment_mask = alignment - 1;

  while ( area != end ) {
    uintptr_t alloc_begin;
    uintptr_t alloc_end;

    alloc_begin = (uintptr_t) area->free;
    alloc_begin += alignment_mask;
    alloc_begin &= ~alignment_mask;
    alloc_end = alloc_begin + size;

    if ( alloc_begin <= alloc_end && alloc_end <= (uintptr_t) area->end ) {
      area->free = (void *) alloc_end;

      return (void *) alloc_begin;
    }

    ++area;
  }

  return NULL;
}
