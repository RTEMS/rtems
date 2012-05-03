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

  MSBUMP(malloc_calls, 1);

  /*
   *  If some free's have been deferred, then do them now.
   */
  malloc_deferred_frees_process();

  /*
   * Validate the parameters
   */
  if ( !size )
    return (void *) 0;

  /*
   *  Do not attempt to allocate memory if not in correct system state.
   */
  if ( _System_state_Is_up(_System_state_Get()) &&
       !malloc_is_system_state_OK() )
    return NULL;

  /*
   * Try to give a segment in the current heap if there is not
   * enough space then try to grow the heap.
   * If this fails then return a NULL pointer.
   */

  return_this = _Protected_heap_Allocate( RTEMS_Malloc_Heap, size );

  if ( !return_this ) {
    if (rtems_malloc_sbrk_helpers)
      return_this = (*rtems_malloc_sbrk_helpers->extend)( size );
    if ( !return_this ) {
      errno = ENOMEM;
      return (void *) 0;
    }
  }

  /*
   *  If the user wants us to dirty the allocated memory, then do it.
   */
  if ( rtems_malloc_dirty_helper )
    (*rtems_malloc_dirty_helper)( return_this, size );

  /*
   *  If configured, update the statistics
   */
  if ( rtems_malloc_statistics_helpers )
    (*rtems_malloc_statistics_helpers->at_malloc)(return_this);

  return return_this;
}

#endif
