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
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"

void *rtems_heap_allocate_aligned_with_boundary(
  size_t size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  if (
    _System_state_Is_up( _System_state_Get() )
      && !malloc_is_system_state_OK()
  ) {
    return NULL;
  }

  malloc_deferred_frees_process();

  /* FIXME: Statistics, boundary checks */

  return _Protected_heap_Allocate_aligned_with_boundary(
    RTEMS_Malloc_Heap,
    size,
    alignment,
    boundary
  );
}

#endif
