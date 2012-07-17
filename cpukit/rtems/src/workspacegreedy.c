/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/support.h>
#include <rtems/score/wkspace.h>

void *rtems_workspace_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
)
{
  void *opaque;

  _Thread_Disable_dispatch();
  opaque = _Heap_Greedy_allocate( &_Workspace_Area, block_sizes, block_count );
  _Thread_Enable_dispatch();

  return opaque;
}

void rtems_workspace_greedy_free( void *opaque )
{
  _Thread_Disable_dispatch();
  _Heap_Greedy_free( &_Workspace_Area, opaque );
  _Thread_Enable_dispatch();
}
