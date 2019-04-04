/**
 *  @file
 *
 *  @brief Workspace Handler Support
 *  @ingroup RTEMSScoreWorkspace
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
#include <rtems/score/assert.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/percpudata.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/tls.h>
#include <rtems/posix/pthread.h>
#include <rtems/config.h>

#include <string.h>

/* #define DEBUG_WORKSPACE */
#if defined(DEBUG_WORKSPACE)
  #include <rtems/bspIo.h>
#endif

RTEMS_LINKER_RWSET(
  _Per_CPU_Data,
  RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES ) char
);

Heap_Control _Workspace_Area;

static uintptr_t _Workspace_Space_for_TLS( uintptr_t page_size )
{
  uintptr_t tls_size;
  uintptr_t space;

  tls_size = _TLS_Get_size();

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
    space = _Heap_Min_block_size( page_size );

    space += _Thread_Initial_thread_count
      * _Heap_Size_with_overhead( page_size, tls_alloc, tls_align );
  } else {
    space = 0;
  }

  return space;
}

#ifdef RTEMS_SMP
static void *_Workspace_Allocate_from_areas(
  Heap_Area *areas,
  size_t     area_count,
  uintptr_t  size,
  uintptr_t  alignment
)
{
  size_t i;

  for ( i = 0; i < area_count; ++i ) {
    Heap_Area *area;
    uintptr_t  alloc_begin;
    uintptr_t  alloc_size;

    area = &areas[ i ];
    alloc_begin = (uintptr_t) area->begin;
    alloc_begin = ( alloc_begin + alignment - 1 ) & ~( alignment - 1 );
    alloc_size = size;
    alloc_size += alloc_begin - (uintptr_t) area->begin;

    if ( area->size >= alloc_size ) {
      area->begin = (void *) ( alloc_begin + size );
      area->size -= alloc_size;

      return (void *) alloc_begin;
    }
  }

  return NULL;
}
#endif

static void _Workspace_Allocate_per_CPU_data(
  Heap_Area *areas,
  size_t area_count
)
{
#ifdef RTEMS_SMP
  uintptr_t size;

  size = RTEMS_LINKER_SET_SIZE( _Per_CPU_Data );

  if ( size > 0 ) {
    Per_CPU_Control *cpu;
    uint32_t         cpu_index;
    uint32_t         cpu_max;

    cpu = _Per_CPU_Get_by_index( 0 );
    cpu->data = RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data );

    cpu_max = rtems_configuration_get_maximum_processors();

    for ( cpu_index = 1 ; cpu_index < cpu_max ; ++cpu_index ) {
      cpu = _Per_CPU_Get_by_index( cpu_index );
      cpu->data = _Workspace_Allocate_from_areas(
        areas,
        area_count,
        size,
        CPU_CACHE_LINE_BYTES
      );

      if( cpu->data == NULL ) {
        _Internal_error( INTERNAL_ERROR_NO_MEMORY_FOR_PER_CPU_DATA );
      }

      memcpy( cpu->data, RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data ), size);
    }
  }
#else
  (void) areas;
  (void) area_count;
#endif
}

void _Workspace_Handler_initialization(
  Heap_Area *areas,
  size_t area_count,
  Heap_Initialization_or_extend_handler extend
)
{
  Heap_Initialization_or_extend_handler init_or_extend;
  uintptr_t                             remaining;
  bool                                  do_zero;
  bool                                  unified;
  uintptr_t                             page_size;
  uintptr_t                             overhead;
  size_t                                i;

  _Workspace_Allocate_per_CPU_data( areas, area_count );

  page_size = CPU_HEAP_ALIGNMENT;

  remaining = rtems_configuration_get_work_space_size();
  remaining += _Workspace_Space_for_TLS( page_size );

  init_or_extend = _Heap_Initialize;
  do_zero = rtems_configuration_get_do_zero_of_workspace();
  unified = rtems_configuration_get_unified_work_area();
  overhead = _Heap_Area_overhead( page_size );

  for ( i = 0; i < area_count; ++i ) {
    Heap_Area *area;

    area = &areas[ i ];

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

      space_available = ( *init_or_extend )(
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
    _Internal_error( INTERNAL_ERROR_TOO_LITTLE_WORKSPACE );
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
    _Internal_error( INTERNAL_ERROR_WORKSPACE_ALLOCATION );

  return memory;
}
