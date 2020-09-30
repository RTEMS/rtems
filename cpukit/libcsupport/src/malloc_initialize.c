/**
 * @file
 *
 * @brief RTEMS_Malloc_Initialize() implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/malloc.h>
#include <rtems/score/wkspace.h>

#include "malloc_p.h"

#ifdef RTEMS_NEWLIB
static Heap_Control _Malloc_Heap;

Heap_Control *RTEMS_Malloc_Initialize(
  const Memory_Information              *mem,
  Heap_Initialization_or_extend_handler  extend
)
{
  Heap_Control                          *heap;
  Heap_Initialization_or_extend_handler  init_or_extend;
  uintptr_t                              page_size;
  size_t                                 i;

  heap = &_Malloc_Heap;
  RTEMS_Malloc_Heap = heap;
  init_or_extend = _Heap_Initialize;
  page_size = CPU_HEAP_ALIGNMENT;

  for (i = 0; i < _Memory_Get_count( mem ); ++i) {
    Memory_Area *area;
    uintptr_t    space_available;

    area = _Memory_Get_area( mem, i );
    space_available = ( *init_or_extend )(
      heap,
      _Memory_Get_free_begin( area ),
      _Memory_Get_free_size( area ),
      page_size
    );

    if ( space_available > 0 ) {
      _Memory_Consume( area, _Memory_Get_free_size( area ) );
      init_or_extend = extend;
    }
  }

  if ( init_or_extend == _Heap_Initialize ) {
    _Internal_error( INTERNAL_ERROR_NO_MEMORY_FOR_HEAP );
  }

  return heap;
}
#else
Heap_Control *RTEMS_Malloc_Initialize(
  const Memory_Information              *mem,
  Heap_Initialization_or_extend_handler  extend
)
{
  /* FIXME: Dummy function */
  return NULL;
}
#endif

Heap_Control *_Workspace_Malloc_initialize_separate( void )
{
  return RTEMS_Malloc_Initialize( _Memory_Get(), _Heap_Extend );
}
