/**
 *  @file
 *
 *  @brief RTEMS Get Region Segment Size
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/regionimpl.h>

rtems_status_code rtems_region_get_segment_size(
  rtems_id   id,
  void      *segment,
  uintptr_t *size
)
{
  rtems_status_code  status;
  Region_Control    *the_region;

  if ( segment == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( size == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  _RTEMS_Lock_allocator();

  the_region = _Region_Get( id );

  if ( the_region != NULL ) {
    if ( _Heap_Size_of_alloc_area( &the_region->Memory, segment, size ) ) {
      status = RTEMS_SUCCESSFUL;
    } else {
      status = RTEMS_INVALID_ADDRESS;
    }
  } else {
    status = RTEMS_INVALID_ID;
  }

  _RTEMS_Unlock_allocator();
  return status;
}
