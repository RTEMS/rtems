/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup MallocSupport
 *
 * @brief This header file provides the implementation of
 *   _Malloc_Initialize_for_multiple_areas().
 */

/*
 * Copyright (C) 2012, 2020 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_MALLOCINITMULTI_H
#define _RTEMS_MALLOCINITMULTI_H

#include <rtems/malloc.h>
#include <rtems/score/heapimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup MallocSupport
 *
 * @brief Initializes the separate C Program Heap with support for more than
 *   one memory area.
 *
 * This implementation should be used by BSPs which provide more than one
 * memory area via _Memory_Get() to implement
 * _Workspace_Malloc_initialize_separate().
 */
static inline Heap_Control *_Malloc_Initialize_for_multiple_areas(
  Heap_Control *heap
)
{
  const Memory_Information              *mem;
  Heap_Initialization_or_extend_handler  init_or_extend;
  uintptr_t                              page_size;
  size_t                                 i;

  mem = _Memory_Get();
  RTEMS_Malloc_Heap = heap;
  init_or_extend = _Heap_Initialize;
  page_size = CPU_HEAP_ALIGNMENT;

  for (i = 0; i < _Memory_Get_count( mem ); ++i) {
    Memory_Area *area;
    uintptr_t    space_available;

    area = _Memory_Get_area( mem, i );
    space_available = ( *init_or_extend )(
      heap,
      _Memory_Get_free_begin( area ),
      _Memory_Get_free_size( area ),
      page_size
    );

    if ( space_available > 0 ) {
      _Memory_Consume( area, _Memory_Get_free_size( area ) );
      init_or_extend = _Heap_Extend;
    }
  }

  if ( init_or_extend == _Heap_Initialize ) {
    _Internal_error( INTERNAL_ERROR_NO_MEMORY_FOR_HEAP );
  }

  return heap;
}

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_MALLOCINITMULTI_H */
