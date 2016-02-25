/**
 *  @file
 *
 *  @brief RTEMS Variation on Aligned Memory Allocation
 *  @ingroup MallocSupport
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"

#include <stdlib.h>
#include <errno.h>

int rtems_memalign(
  void   **pointer,
  size_t   alignment,
  size_t   size
)
{
  void *return_this;

  /*
   *  Parameter error checks
   */
  if ( !pointer )
    return EINVAL;

  *pointer = NULL;

  /*
   *  Perform the aligned allocation requested
   */
  return_this = rtems_heap_allocate_aligned_with_boundary( size, alignment, 0 );
  if ( !return_this )
    return ENOMEM;

  *pointer = return_this;
  return 0;
}
#endif
