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
  if ( !malloc_is_system_state_OK() )
    return NULL;

  /*
   * Try to give a segment in the current heap if there is not
   * enough space then try to grow the heap.
   * If this fails then return a NULL pointer.
   */

  return_this = _Protected_heap_Allocate( RTEMS_Malloc_Heap, size );

  if ( !return_this ) {
    return_this = (*rtems_malloc_extend_handler)( RTEMS_Malloc_Heap, size );
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

  return return_this;
}

#endif
