/*
 *  Workspace Handler
 *
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

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/interr.h>

#include <string.h>  /* for memset */

/* #define DEBUG_WORKSPACE */
#if defined(DEBUG_WORKSPACE)
  #include <rtems/bspIo.h>
#endif

/*
 *  _Workspace_Handler_initialization
 */
void _Workspace_Handler_initialization(void)
{
  uintptr_t memory_available = 0;
  void *starting_address = rtems_configuration_get_work_space_start();
  uintptr_t size = rtems_configuration_get_work_space_size();

  if ( rtems_configuration_get_do_zero_of_workspace() )
    memset( starting_address, 0, size );

  memory_available = _Heap_Initialize(
    &_Workspace_Area,
    starting_address,
    size,
    CPU_HEAP_ALIGNMENT
  );

  if ( memory_available == 0 )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_TOO_LITTLE_WORKSPACE
    );
}

/*
 *  _Workspace_Allocate
 */
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

/*
 *  _Workspace_Allocate_or_fatal_error
 */
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
