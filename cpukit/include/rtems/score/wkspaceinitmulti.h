/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief This header file provides the implementation of
 *   _Workspace_Initialize_for_multiple_areas().
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

#ifndef _RTEMS_SCORE_WKSPACEINITMULTI_H
#define _RTEMS_SCORE_WKSPACEINITMULTI_H

#include <rtems/score/wkspace.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/memory.h>
#include <rtems/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief Initializes the RTEMS Workspace with support for more than one memory
 *   area.
 *
 * This implementation should be used by BSPs which provide more than one
 * memory area via _Memory_Get() to implement
 * _Workspace_Handler_initialization().
 */
static inline void _Workspace_Initialize_for_multiple_areas( void )
{
  const Memory_Information             *mem;
  Heap_Initialization_or_extend_handler init_or_extend;
  uintptr_t                             remaining;
  bool                                  unified;
  uintptr_t                             page_size;
  uintptr_t                             overhead;
  size_t                                i;

  mem = _Memory_Get();
  page_size = CPU_HEAP_ALIGNMENT;
  remaining = rtems_configuration_get_work_space_size();
  init_or_extend = _Heap_Initialize;
  unified = rtems_configuration_get_unified_work_area();
  overhead = _Heap_Area_overhead( page_size );

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    Memory_Area *area;
    uintptr_t    free_size;

    area = _Memory_Get_area( mem, i );
    free_size = _Memory_Get_free_size( area );

    if ( free_size > overhead ) {
      uintptr_t space_available;
      uintptr_t size;

      if ( unified ) {
        size = free_size;
      } else {
        if ( remaining > 0 ) {
          size = remaining < free_size - overhead ?
            remaining + overhead : free_size;
        } else {
          size = 0;
        }
      }

      space_available = ( *init_or_extend )(
        &_Workspace_Area,
        _Memory_Get_free_begin( area ),
        size,
        page_size
      );

      _Memory_Consume( area, size );

      if ( space_available < remaining ) {
        remaining -= space_available;
      } else {
        remaining = 0;
      }

      init_or_extend = _Heap_Extend;
    }
  }

  if ( remaining > 0 ) {
    _Internal_error( INTERNAL_ERROR_TOO_LITTLE_WORKSPACE );
  }

  _Heap_Protection_set_delayed_free_fraction( &_Workspace_Area, 1 );
}

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_WKSPACEINITMULTI_H */
