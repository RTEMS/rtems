/**
 *  @file
 *
 *  @brief RTEMS Malloc Get Heap Pointer
 *  @ingroup libcsupport
 */

/*
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

#include <rtems.h>
#include <rtems/libcsupport.h>
#include "malloc_p.h"

Heap_Control *malloc_get_heap_pointer( void )
{
  return RTEMS_Malloc_Heap;
}
