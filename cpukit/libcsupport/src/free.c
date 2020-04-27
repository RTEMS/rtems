/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief calloc()
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

void free(
  void *ptr
)
{
  if ( !ptr )
    return;

  /*
   *  Do not attempt to free memory if in a critical section or ISR.
   */
  if ( _Malloc_System_state() != MALLOC_SYSTEM_STATE_NORMAL ) {
      _Malloc_Deferred_free(ptr);
      return;
  }

  if ( !_Protected_heap_Free( RTEMS_Malloc_Heap, ptr ) ) {
    rtems_fatal( RTEMS_FATAL_SOURCE_INVALID_HEAP_FREE, (rtems_fatal_code) ptr );
  }
}
#endif
