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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/malloc.h>

#include "malloc_p.h"

#ifdef RTEMS_NEWLIB
void RTEMS_Malloc_Initialize(
  const Heap_Area *areas,
  size_t area_count,
  Heap_Initialization_or_extend_handler extend
)
{
  Heap_Control *heap = RTEMS_Malloc_Heap;

  if ( !rtems_configuration_get_unified_work_area() ) {
    Heap_Initialization_or_extend_handler init_or_extend = _Heap_Initialize;
    uintptr_t page_size = CPU_HEAP_ALIGNMENT;
    size_t i;

    for (i = 0; i < area_count; ++i) {
      const Heap_Area *area = &areas [i];
      uintptr_t space_available = (*init_or_extend)(
        heap,
        area->begin,
        area->size,
        page_size
      );

      if ( space_available > 0 ) {
        init_or_extend = extend;
      }
    }

    if ( init_or_extend == _Heap_Initialize ) {
      _Internal_error( INTERNAL_ERROR_NO_MEMORY_FOR_HEAP );
    }
  }
}
#else
void RTEMS_Malloc_Initialize(
  Heap_Area *areas,
  size_t area_count,
  Heap_Initialization_or_extend_handler extend
)
{
  /* FIXME: Dummy function */
}
#endif
