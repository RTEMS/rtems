/*
 *  malloc_walk Implementation
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

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"

#include <stdlib.h>

bool malloc_walk(int source, bool printf_enabled)
{
  return _Protected_heap_Walk( RTEMS_Malloc_Heap, source, printf_enabled );
}

#endif
