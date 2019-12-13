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
#include <rtems/score/threadimpl.h>
#include <rtems/score/tls.h>
#include <rtems/posix/pthread.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>

#include <string.h>

/* #define DEBUG_WORKSPACE */
#if defined(DEBUG_WORKSPACE)
  #include <rtems/bspIo.h>
#endif

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

static void _Workspace_Initialize( void )
{
  _Workspace_Handler_initialization( _Memory_Get(), _Heap_Extend );
}

RTEMS_SYSINIT_ITEM(
  _Workspace_Initialize,
  RTEMS_SYSINIT_WORKSPACE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

void _Workspace_Handler_initialization(
  const Memory_Information              *mem,
  Heap_Initialization_or_extend_handler  extend
)
{
  Heap_Initialization_or_extend_handler init_or_extend;
  uintptr_t                             remaining;
  bool                                  do_zero;
  bool                                  unified;
  uintptr_t                             page_size;
  uintptr_t                             overhead;
  size_t                                i;

  page_size = CPU_HEAP_ALIGNMENT;
  remaining = rtems_configuration_get_work_space_size();
  remaining += _Workspace_Space_for_TLS( page_size );

  init_or_extend = _Heap_Initialize;
  do_zero = rtems_configuration_get_do_zero_of_workspace();
  unified = rtems_configuration_get_unified_work_area();
  overhead = _Heap_Area_overhead( page_size );

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    Memory_Area *area;
    uintptr_t    free_size;

    area = _Memory_Get_area( mem, i );
    free_size = _Memory_Get_free_size( area );

    if ( do_zero ) {
      memset( _Memory_Get_free_begin( area ), 0, free_size );
    }

    if ( free_size > overhead ) {
      uintptr_t space_available;
      uintptr_t size;

      if ( unified ) {
        size = free_size;
      } else {
        if ( remaining > 0 ) {
          size = remaining < free_size - overhead ?
            remaining + overhead : free_size;
        } else {
          size = 0;
        }
      }

      space_available = ( *init_or_extend )(
        &_Workspace_Area,
        _Memory_Get_free_begin( area ),
        size,
        page_size
      );

      _Memory_Consume( area, size );

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
