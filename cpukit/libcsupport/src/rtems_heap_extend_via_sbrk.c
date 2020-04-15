/**
 *  @file
 *
 *  @brief RTEMS Extend Heap via Sbrk
 *  @ingroup MallocSupport
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

#include <unistd.h>

#include <rtems/malloc.h>

#include "malloc_p.h"

ptrdiff_t RTEMS_Malloc_Sbrk_amount;

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
