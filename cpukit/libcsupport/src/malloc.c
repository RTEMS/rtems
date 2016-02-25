/**
 *  @file
 *
 *  @brief RTEMS Malloc Family Implementation
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <stdlib.h>
#include <errno.h>

#include "malloc_p.h"

void *malloc(
  size_t  size
)
{
  void        *return_this;

  /*
   * Validate the parameters
   */
  if ( !size )
    return (void *) 0;

  return_this = rtems_heap_allocate_aligned_with_boundary( size, 0, 0 );
  if ( !return_this ) {
    errno = ENOMEM;
    return (void *) 0;
  }

  return return_this;
}

#endif
