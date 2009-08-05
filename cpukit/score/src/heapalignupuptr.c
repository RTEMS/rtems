/*
 *  Heap Handler
 *
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/heap.h>

/*
 * This routine is NOT inlined because it has a branch which leads to
 * path explosion where it is used.  This makes full test coverage more
 * difficult.
 */
void _Heap_Align_up_uptr (
  _H_uptr_t *value,
  uint32_t  alignment
)
{
  _H_uptr_t remainder;
  _H_uptr_t v = *value;

  remainder = v % alignment;

  if ( remainder )
    *value = v - remainder + alignment;
}


