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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/wkspace.h>
#include <rtems/score/assert.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/interr.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>

Heap_Control _Workspace_Area;

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
  bool                                  unified;
  uintptr_t                             page_size;
  uintptr_t                             overhead;
  size_t                                i;

  page_size = CPU_HEAP_ALIGNMENT;
  remaining = rtems_configuration_get_work_space_size();
  init_or_extend = _Heap_Initialize;
  unified = rtems_configuration_get_unified_work_area();
  overhead = _Heap_Area_overhead( page_size );

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    Memory_Area *area;
    uintptr_t    free_size;

    area = _Memory_Get_area( mem, i );
    free_size = _Memory_Get_free_size( area );

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

void *_Workspace_Allocate( size_t size )
{
  return _Heap_Allocate( &_Workspace_Area, size );
}

void _Workspace_Free( void *block )
{
  _Heap_Free( &_Workspace_Area, block );
}
