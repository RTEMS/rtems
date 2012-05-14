/**
 * @file
 *
 * @brief Malloc initialization implementation.
 */

/*
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

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/score/wkspace.h>
#include "malloc_p.h"

/* FIXME: Dummy function */
#ifndef RTEMS_NEWLIB
void RTEMS_Malloc_Initialize(
  void *heap_begin,
  uintptr_t heap_size,
  size_t sbrk_amount
)
{
}
#else
rtems_malloc_statistics_t rtems_malloc_statistics;

void RTEMS_Malloc_Initialize(
  void *heap_begin,
  uintptr_t heap_size,
  size_t sbrk_amount
)
{
  bool separate_areas = !rtems_configuration_get_unified_work_area();
  /*
   *  If configured, initialize the statistics support
   */
  if ( rtems_malloc_statistics_helpers != NULL ) {
    (*rtems_malloc_statistics_helpers->initialize)();
  }

  /*
   *  Initialize the garbage collection list to start with nothing on it.
   */
  malloc_deferred_frees_initialize();

  /*
   *  Initialize the optional sbrk support for extending the heap
   */
  if ( rtems_malloc_sbrk_helpers != NULL ) {
    void *new_heap_begin = (*rtems_malloc_sbrk_helpers->initialize)(
      heap_begin,
      sbrk_amount
    );

    heap_size -= (uintptr_t) new_heap_begin - (uintptr_t) heap_begin;
    heap_begin = new_heap_begin;
  }

  /*
   *  If this system is configured to use the same heap for
   *  the RTEMS Workspace and C Program Heap, then we need to
   *  be very very careful about destroying the initialization
   *  that has already been done.
   */

  /*
   *  If the BSP is not clearing out the workspace, then it is most likely
   *  not clearing out the initial memory for the heap.  There is no
   *  standard supporting zeroing out the heap memory.  But much code
   *  with UNIX history seems to assume that memory malloc'ed during
   *  initialization (before any free's) is zero'ed.  This is true most
   *  of the time under UNIX because zero'ing memory when it is first
   *  given to a process eliminates the chance of a process seeing data
   *  left over from another process.  This would be a security violation.
   */

  if ( separate_areas && rtems_configuration_get_do_zero_of_workspace() ) {
     memset( heap_begin, 0, heap_size );
  }

  /*
   *  Unfortunately we cannot use assert if this fails because if this
   *  has failed we do not have a heap and if we do not have a heap
   *  STDIO cannot work because there will be no buffers.
   */

  if ( separate_areas ) {
    uintptr_t status = _Protected_heap_Initialize(
      RTEMS_Malloc_Heap,
      heap_begin,
      heap_size,
      CPU_HEAP_ALIGNMENT
    );
    if ( status == 0 ) {
      rtems_fatal_error_occurred( RTEMS_NO_MEMORY );
    }
  }

  MSBUMP( space_available, _Protected_heap_Get_size(RTEMS_Malloc_Heap) );
}
#endif
