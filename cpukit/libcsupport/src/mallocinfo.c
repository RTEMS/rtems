/*
 *  RTEMS Malloc Get Status Information
 *
 *
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/protectedheap.h>

extern Heap_Control  *RTEMS_Malloc_Heap;

/*
 *  Find amount of free heap remaining
 */

int malloc_info(
  Heap_Information_block *the_info
)
{
  if ( !the_info )
    return -1;

  _Protected_heap_Get_information( RTEMS_Malloc_Heap, the_info );
  return 0;
}
