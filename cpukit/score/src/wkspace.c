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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/wkspace.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/interr.h>
#include <rtems/config.h>

#include <string.h>  /* for memset */

/* #define DEBUG_WORKSPACE */
#if defined(DEBUG_WORKSPACE)
  #include <rtems/bspIo.h>
#endif

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
  size_t i;

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
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_TOO_LITTLE_WORKSPACE
    );
  }
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
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_WORKSPACE_ALLOCATION
    );

  return memory;
}
