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

void free(
  void *ptr
)
{
  MSBUMP(free_calls, 1);

  if ( !ptr )
    return;

  /*
   *  Do not attempt to free memory if in a critical section or ISR.
   */
  if ( _System_state_Is_up(_System_state_Get()) &&
       !malloc_is_system_state_OK() ) {
      malloc_deferred_free(ptr);
      return;
  }

  /*
   *  If configured, update the statistics
   */
  if ( rtems_malloc_statistics_helpers )
    (*rtems_malloc_statistics_helpers->at_free)(ptr);

  if ( !_Protected_heap_Free( RTEMS_Malloc_Heap, ptr ) ) {
    printk( "Program heap: free of bad pointer %p -- range %p - %p \n",
      ptr,
      RTEMS_Malloc_Heap->area_begin,
      RTEMS_Malloc_Heap->area_end
    );
  }

}
#endif
