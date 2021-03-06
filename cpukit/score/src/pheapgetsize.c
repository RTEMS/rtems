/**
 * @file
 *
 * @ingroup RTEMSScoreProtHeap
 *
 * @brief This source file contains the implementation of
 *   _Protected_heap_Get_size().
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/protectedheap.h>

uintptr_t _Protected_heap_Get_size(
  Heap_Control *the_heap
)
{
  return _Heap_Get_size( the_heap );
}
