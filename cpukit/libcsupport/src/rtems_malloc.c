/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief rtems_malloc() implementation.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"

#include <stdlib.h>
#include <errno.h>

void *rtems_malloc(size_t size, uintptr_t alignment, uintptr_t boundary)
{
  void *p = NULL;

  _RTEMS_Lock_allocator();
   p = _Heap_Allocate_aligned_with_boundary(
     RTEMS_Malloc_Heap,
     size,
     alignment,
     boundary
   );
  _RTEMS_Unlock_allocator();

  return p;
}

#endif
