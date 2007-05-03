/**
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/protectedheap.h>

boolean _Protected_heap_Walk(
  Heap_Control *the_heap,
  int           source,
  boolean       do_dump
)
{
  boolean status;

  _RTEMS_Lock_allocator();
    status = _Heap_Walk( the_heap, source, do_dump );
  _RTEMS_Unlock_allocator();
  return status;
}
