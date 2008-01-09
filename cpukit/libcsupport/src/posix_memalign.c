/*
 *  posix_memalign()
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
#include "malloc_p.h"

#include <stdlib.h>
#include <errno.h>

int posix_memalign(
  void   **pointer,
  size_t   alignment,
  size_t   size
)
{
  void *return_this;

  /*
   *  Update call statistics
   */
  MSBUMP(memalign_calls, 1);

  /*
   *  Parameter error checks
   */
  if ( !pointer )
    return EINVAL;

  *pointer = NULL;

  if (((alignment - 1) & alignment) != 0 || (alignment < sizeof(void *)))
    return EINVAL;

  if ( !size )
    return EINVAL;

  /*
   *  Do not attempt to allocate memory if not in correct system state.
   */
  if ( _System_state_Is_up(_System_state_Get()) && 
       !malloc_is_system_state_OK() )
    return EINVAL;

  /*
   *
   *  If some free's have been deferred, then do them now.
   */
  malloc_deferred_frees_process();

  #if defined(RTEMS_MALLOC_BOUNDARY_HELPERS)
    /*
     *  If the support for a boundary area at the end of the heap
     *  block allocated is turned on, then adjust the size.
     */
    if (rtems_malloc_boundary_helpers)
      size += (*rtems_malloc_boundary_helpers->overhead)();
  #endif

  /*
   *  Perform the aligned allocation requested
   */

  return_this = _Protected_heap_Allocate_aligned(
    &RTEMS_Malloc_Heap,
    size,
    alignment
  );
  if ( !return_this )
    return ENOMEM;

  /*
   *  If configured, update the more involved statistics
   */
  if ( rtems_malloc_statistics_helpers )
    (*rtems_malloc_statistics_helpers->at_malloc)(pointer);

  #if defined(RTEMS_MALLOC_BOUNDARY_HELPERS)
    /*
     * If configured, set the boundary area
     */
    if (rtems_malloc_boundary_helpers)
      (*rtems_malloc_boundary_helpers->at_malloc)(return_this, size);
  #endif

  *pointer = return_this; 
  return 0;
}
#endif
