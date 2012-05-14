/*
 *  RTEMS Malloc Get Free Information
 *
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
#include <rtems/score/protectedheap.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include "malloc_p.h"

/*
 *  Find amount of free heap remaining
 */

size_t malloc_free_space( void )
{
  Heap_Information info;

  _Protected_heap_Get_free_information( RTEMS_Malloc_Heap, &info );
  return (size_t) info.largest;
}
