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

boolean _Protected_heap_Get_block_size(
  Heap_Control        *the_heap,
  void                *starting_address,
  size_t              *size
)
{
  boolean status;

  _RTEMS_Lock_allocator();
    status = _Heap_Size_of_user_area( the_heap, starting_address, size );
  _RTEMS_Unlock_allocator();
  return status;
}
