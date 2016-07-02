/*
 *  RTEMS Cache Aligned Malloc
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/malloc.h>

void *rtems_cache_aligned_malloc( size_t nbytes )
{
  size_t line_size = rtems_cache_get_maximal_line_size();

  if ( line_size > 0 ) {
    /* Assume that the cache line size is a power of two */
    size_t m = line_size - 1;

    nbytes = (nbytes + m) & ~m;
  }

  return rtems_heap_allocate_aligned_with_boundary( nbytes, line_size, 0 );
}
