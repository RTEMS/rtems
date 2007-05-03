/*
 *  RTEMS Malloc Get Status Information
 *
 *
 *  COPYRIGHT (c) 1989-2000.
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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/protectedheap.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

extern Heap_Control  RTEMS_Malloc_Heap;

/*
 *  Find amount of free heap remaining
 */

size_t malloc_free_space( void )
{
  Heap_Information info;

  _Protected_heap_Get_free_information( &RTEMS_Malloc_Heap, &info );
  return (size_t) info.largest;
}
