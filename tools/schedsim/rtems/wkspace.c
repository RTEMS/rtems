/*
 *  BASED UPON SOURCE IN RTEMS, MODIFIED FOR SIMULATOR
 *
 *  Workspace Handler
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/interr.h>

#include <stdlib.h>

#if defined(DEBUG_WORKSPACE)
  #include <stdio.h>
#endif

/*
 *  _Workspace_Handler_initialization
 */
void _Workspace_Handler_initialization(void)
{
}

/*
 *  _Workspace_Allocate
 */
void *_Workspace_Allocate(
  size_t   size
)
{
  void *memory;

  memory = calloc( 1, size );
  #if defined(DEBUG_WORKSPACE)
    fprintf(
      stderr,
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
    fprintf(
      stderr,
      block,
      __builtin_return_address( 0 ),
      __builtin_return_address( 1 )
    );
  #endif
  free( block );
}

/*
 *  _Workspace_Allocate_or_fatal_error
 */
void *_Workspace_Allocate_or_fatal_error(
  size_t      size
)
{
  void *memory;

  memory = calloc( 1, size );
  #if defined(DEBUG_WORKSPACE)
    fprintf(
      stderr,
      "Workspace_Allocate_or_fatal_error(%d) from %p/%p -> %p\n",
      size,
      __builtin_return_address( 0 ),
      __builtin_return_address( 1 ),
      memory
    );
  #endif

  return memory;
}
