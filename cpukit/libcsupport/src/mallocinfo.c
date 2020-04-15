/**
 *  @file
 *
 *  @brief RTEMS Malloc Get Status Information
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/malloc.h>
#include <rtems/score/protectedheap.h>

int malloc_info(
  Heap_Information_block *the_info
)
{
  if ( !the_info )
    return -1;

  _Protected_heap_Get_information( RTEMS_Malloc_Heap, the_info );
  return 0;
}
