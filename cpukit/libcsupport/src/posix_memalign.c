/**
 * @file
 *
 * @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"

#include <stdlib.h>
#include <errno.h>

int posix_memalign(
  void   **memptr,
  size_t   alignment,
  size_t   size
)
{
  RTEMS_OBFUSCATE_VARIABLE( memptr );

  if ( memptr == NULL ) {
    return EINVAL;
  }

  *memptr = NULL;

  if ( alignment < sizeof( void * ) ) {
    return EINVAL;
  }

  if ( ( ( alignment - 1 ) & alignment ) != 0 ) {
    return EINVAL;
  }

  if ( size == 0 ) {
    return 0;
  }

  *memptr = rtems_heap_allocate_aligned_with_boundary( size, alignment, 0 );

  if ( *memptr == NULL ) {
    return ENOMEM;
  }

  return 0;
}
#endif
