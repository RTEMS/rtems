/*
 *  rtems_memalign() - Raw aligned allocate from Protected Heap
 *
 *  COPYRIGHT (c) 1989-2008.
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
   *  Do not attempt to allocate memory if not in correct system state.
   */
  if ( _System_state_Is_up(_System_state_Get()) &&
       !malloc_is_system_state_OK() )
    return EINVAL;

  /*
   *  If some free's have been deferred, then do them now.
   */
  malloc_deferred_frees_process();

  /*
   *  Perform the aligned allocation requested
   */
  return_this = _Protected_heap_Allocate_aligned(
    RTEMS_Malloc_Heap,
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

  *pointer = return_this;
  return 0;
}
#endif
