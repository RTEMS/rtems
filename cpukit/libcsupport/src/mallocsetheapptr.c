/*
 *  RTEMS Malloc Set Heap Pointer -- Primarily for Debug
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <rtems/libcsupport.h>
#include "malloc_p.h"

void malloc_set_heap_pointer(
  Heap_Control *new_heap
)
{
  RTEMS_Malloc_Heap = new_heap;
}
