/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief RTEMS Extend Heap via Sbrk
 *  @ingroup MallocSupport
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include <unistd.h>

#include <rtems/malloc.h>

#include "malloc_p.h"

static ptrdiff_t RTEMS_Malloc_Sbrk_amount;

void rtems_heap_set_sbrk_amount( ptrdiff_t sbrk_amount )
{
  RTEMS_Malloc_Sbrk_amount = sbrk_amount;
}

void *rtems_heap_extend_via_sbrk(
  Heap_Control *heap,
  size_t alloc_size
)
{
  ptrdiff_t sbrk_amount = RTEMS_Malloc_Sbrk_amount;
  ptrdiff_t sbrk_size = (ptrdiff_t) alloc_size;
  ptrdiff_t misaligned = sbrk_amount != 0 ? sbrk_size % sbrk_amount : 0;
  void *return_this = NULL;

  if ( misaligned != 0 ) {
    sbrk_size += sbrk_amount - misaligned;
  }

  if ( sbrk_size > 0 && sbrk_amount > 0 ) {
    void *area_begin = sbrk( sbrk_size );

    if ( area_begin != (void *) -1 ) {
      bool ok = _Protected_heap_Extend( heap, area_begin, sbrk_size );

      if ( ok ) {
        return_this = _Protected_heap_Allocate( heap, alloc_size );
      } else {
        sbrk( -sbrk_size );
      }
    }
  }

  return return_this;
}
