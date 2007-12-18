/*
 *  RTEMS Malloc Family Implementation
 *
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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
  void        *starting_address;
  uint32_t     the_size;
  uint32_t     sbrk_amount;
  Chain_Node  *to_be_freed;

  MSBUMP(malloc_calls, 1);

  if ( !size )
    return (void *) 0;

  #if defined(RTEMS_HEAP_DEBUG)
    _Protected_heap_Walk( &RTEMS_Malloc_Heap, 0, FALSE );
  #endif

  /*
   *  Do not attempt to allocate memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if (_Thread_Dispatch_disable_level > 0)
      return (void *) 0;

    if (_ISR_Nest_level > 0)
      return (void *) 0;
  }

  /*
   *  If some free's have been deferred, then do them now.
   */
  while ((to_be_freed = Chain_Get(&RTEMS_Malloc_GC_list)) != NULL)
    free(to_be_freed);

  #if defined(RTEMS_MALLOC_BOUNDARY_HELPERS)
    /*
     *  If the support for a boundary area at the end of the heap
     *  block allocated is turned on, then adjust the size.
     */
    if (rtems_malloc_boundary_helpers)
      size += (*rtems_malloc_boundary_helpers->overhead)();
  #endif

  /*
   * Try to give a segment in the current heap if there is not
   * enough space then try to grow the heap.
   * If this fails then return a NULL pointer.
   */

  return_this = _Protected_heap_Allocate( &RTEMS_Malloc_Heap, size );

  if ( !return_this ) {
    /*
     *  Round to the "requested sbrk amount" so hopefully we won't have
     *  to grow again for a while.  This effectively does sbrk() calls
     *  in "page" amounts.
     */

    sbrk_amount = RTEMS_Malloc_Sbrk_amount;

    if ( sbrk_amount == 0 )
      return (void *) 0;

    the_size = ((size + sbrk_amount) / sbrk_amount * sbrk_amount);

    if ((starting_address = (void *)sbrk(the_size))
            == (void*) -1)
      return (void *) 0;

    if ( !_Protected_heap_Extend(
            &RTEMS_Malloc_Heap, starting_address, the_size) ) {
      sbrk(-the_size);
      errno = ENOMEM;
      return (void *) 0;
    }

    MSBUMP(space_available, the_size);

    return_this = _Protected_heap_Allocate( &RTEMS_Malloc_Heap, size );
    if ( !return_this ) {
      errno = ENOMEM;
      return (void *) 0;
    }
  }

  /*
   *  If the user wants us to dirty the allocated memory, then do it.
   */
  #ifdef MALLOC_DIRTY
    (void) memset(return_this, 0xCF, size);
  #endif

  /*
   *  If configured, update the statistics
   */
  if ( rtems_malloc_statistics_helpers )
    (*rtems_malloc_statistics_helpers->at_malloc)(return_this);

  #if defined(RTEMS_MALLOC_BOUNDARY_HELPERS)
    /*
     * If configured, set the boundary area
     */
    if (rtems_malloc_boundary_helpers)
      (*rtems_malloc_boundary_helpers->at_malloc)(return_this, size);
  #endif

  return return_this;
}

#endif
