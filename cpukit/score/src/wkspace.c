/**
 *  @file
 *
 *  @brief Workspace Handler Support
 *  @ingroup ScoreWorkspace
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/wkspace.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/tls.h>
#include <rtems/config.h>

#include <string.h>  /* for memset */

/* #define DEBUG_WORKSPACE */
#if defined(DEBUG_WORKSPACE)
  #include <rtems/bspIo.h>
#endif

static uint32_t _Get_maximum_thread_count(void)
{
  uint32_t thread_count = 0;

  thread_count += _Thread_Get_maximum_internal_threads();

  thread_count += rtems_resource_maximum_per_allocation(
    Configuration_RTEMS_API.maximum_tasks
  );

#if defined(RTEMS_POSIX_API)
  thread_count += rtems_resource_maximum_per_allocation(
    Configuration_POSIX_API.maximum_threads
  );
#endif

  return thread_count;
}

void _Workspace_Handler_initialization(
  Heap_Area *areas,
  size_t area_count,
  Heap_Initialization_or_extend_handler extend
)
{
  Heap_Initialization_or_extend_handler init_or_extend = _Heap_Initialize;
  uintptr_t remaining = rtems_configuration_get_work_space_size();
  bool do_zero = rtems_configuration_get_do_zero_of_workspace();
  bool unified = rtems_configuration_get_unified_work_area();
  uintptr_t page_size = CPU_HEAP_ALIGNMENT;
  uintptr_t overhead = _Heap_Area_overhead( page_size );
  uintptr_t tls_size = _TLS_Get_size();
  size_t i;

  /*
   * In case we have a non-zero TLS size, then we need a TLS area for each
   * thread.  These areas are allocated from the workspace.  Ensure that the
   * workspace is large enough to fulfill all requests known at configuration
   * time (so excluding the unlimited option).  It is not possible to estimate
   * the TLS size in the configuration at compile-time.  The TLS size is
   * determined at application link-time.
   */
  if ( tls_size > 0 ) {
    uintptr_t tls_align = _TLS_Heap_align_up( (uintptr_t) _TLS_Alignment );
    uintptr_t tls_alloc = _TLS_Get_allocation_size( tls_size, tls_align );

    /*
     * Memory allocated with an alignment constraint is allocated from the end
     * of a free block.  The last allocation may need one free block of minimum
     * size.
     */
    remaining += _Heap_Min_block_size( page_size );

    remaining += _Get_maximum_thread_count()
      * _Heap_Size_with_overhead( page_size, tls_alloc, tls_align );
  }

  for (i = 0; i < area_count; ++i) {
    Heap_Area *area = &areas [i];

    if ( do_zero ) {
      memset( area->begin, 0, area->size );
    }

    if ( area->size > overhead ) {
      uintptr_t space_available;
      uintptr_t size;

      if ( unified ) {
        size = area->size;
      } else {
        if ( remaining > 0 ) {
          size = remaining < area->size - overhead ?
            remaining + overhead : area->size;
        } else {
          size = 0;
        }
      }

      space_available = (*init_or_extend)(
        &_Workspace_Area,
        area->begin,
        size,
        page_size
      );

      area->begin = (char *) area->begin + size;
      area->size -= size;

      if ( space_available < remaining ) {
        remaining -= space_available;
      } else {
        remaining = 0;
      }

      init_or_extend = extend;
    }
  }

  if ( remaining > 0 ) {
    _Terminate(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_TOO_LITTLE_WORKSPACE
    );
  }

  _Heap_Protection_set_delayed_free_fraction( &_Workspace_Area, 1 );
}

void *_Workspace_Allocate(
  size_t   size
)
{
  void *memory;

  memory = _Heap_Allocate( &_Workspace_Area, size );
  #if defined(DEBUG_WORKSPACE)
    printk(
      "Workspace_Allocate(%d) from %p/%p -> %p\n",
      size,
      __builtin_return_address( 0 ),
      __builtin_return_address( 1 ),
      memory
    );
  #endif
  return memory;
}

void *_Workspace_Allocate_aligned( size_t size, size_t alignment )
{
  return _Heap_Allocate_aligned( &_Workspace_Area, size, alignment );
}

/*
 *  _Workspace_Free
 */
void _Workspace_Free(
  void *block
)
{
  #if defined(DEBUG_WORKSPACE)
    printk(
      "Workspace_Free(%p) from %p/%p\n",
      block,
      __builtin_return_address( 0 ),
      __builtin_return_address( 1 )
    );
  #endif
  _Heap_Free( &_Workspace_Area, block );
}

void *_Workspace_Allocate_or_fatal_error(
  size_t      size
)
{
  void *memory;

  memory = _Heap_Allocate( &_Workspace_Area, size );
  #if defined(DEBUG_WORKSPACE)
    printk(
      "Workspace_Allocate_or_fatal_error(%d) from %p/%p -> %p\n",
      size,
      __builtin_return_address( 0 ),
      __builtin_return_address( 1 ),
      memory
    );
  #endif

  if ( memory == NULL )
    _Terminate(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_WORKSPACE_ALLOCATION
    );

  return memory;
}
