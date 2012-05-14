/*
 *  calloc()
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"
#include <stdlib.h>
#include <errno.h>

void *realloc(
  void *ptr,
  size_t size
)
{
  uintptr_t old_size;
  char    *new_area;

  MSBUMP(realloc_calls, 1);

  /*
   *  Do not attempt to allocate memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if (_Thread_Dispatch_in_critical_section())
      return (void *) 0;

    if (_ISR_Nest_level > 0)
      return (void *) 0;
  }

  /*
   * Continue with realloc().
   */
  if ( !ptr )
    return malloc( size );

  if ( !size ) {
    free( ptr );
    return (void *) 0;
  }

  if ( !_Protected_heap_Get_block_size(RTEMS_Malloc_Heap, ptr, &old_size) ) {
    errno = EINVAL;
    return (void *) 0;
  }

  /*
   *  Now resize it.
   */
  if ( _Protected_heap_Resize_block( RTEMS_Malloc_Heap, ptr, size ) ) {
    return ptr;
  }

  /*
   *  There used to be a free on this error case but it is wrong to
   *  free the memory per OpenGroup Single UNIX Specification V2
   *  and the C Standard.
   */

  new_area = malloc( size );

  MSBUMP(malloc_calls, (uint32_t) -1);   /* subtract off the malloc */

  if ( !new_area ) {
    return (void *) 0;
  }

  memcpy( new_area, ptr, (size < old_size) ? size : old_size );
  free( ptr );

  return new_area;

}
#endif
