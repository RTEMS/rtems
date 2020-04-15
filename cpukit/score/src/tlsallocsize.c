/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2014, 2019 embedded brains GmbH
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

#include <rtems/score/tls.h>

static uintptr_t _TLS_Allocation_size;

uintptr_t _TLS_Get_allocation_size( void )
{
  uintptr_t size;
  uintptr_t allocation_size;
  uintptr_t alignment;

  size = _TLS_Get_size();

  if ( size == 0 ) {
    return 0;
  }

  allocation_size = _TLS_Allocation_size;

  if ( allocation_size == 0 ) {
    allocation_size = _TLS_Heap_align_up( size );
    alignment = _TLS_Heap_align_up( (uintptr_t) _TLS_Alignment );

    /*
     * The stack allocator does not support aligned allocations.  Allocate
     * enough to do the alignment manually.
     */
    if ( alignment > CPU_HEAP_ALIGNMENT ) {
      allocation_size += alignment;
    }

    allocation_size += _TLS_Get_thread_control_block_area_size( alignment );

#ifndef __i386__
    allocation_size += sizeof(TLS_Dynamic_thread_vector);
#endif

    _TLS_Allocation_size = allocation_size;
  }

  return allocation_size;
}
