/**
 *  @file
 *
 *  @brief Resize RTEMS Region Segment
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

rtems_status_code rtems_region_resize_segment(
  rtems_id    id,
  void       *segment,
  uintptr_t   size,
  uintptr_t  *old_size
)
{
  uintptr_t           avail_size;
  uintptr_t           osize;
  rtems_status_code   status;
  Heap_Resize_status  resize_status;
  Region_Control     *the_region;

  if ( old_size == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_region = _Region_Get_and_lock( id );

  if ( the_region == NULL ) {
    return RTEMS_INVALID_ID;
  }

  resize_status = _Heap_Resize_block(
    &the_region->Memory,
    segment,
    (uint32_t) size,
    &osize,
    &avail_size
  );
  *old_size = (uint32_t) osize;

  switch ( resize_status ) {
    case HEAP_RESIZE_SUCCESSFUL:
      /* Unlocks allocator */
      _Region_Process_queue( the_region );
      return RTEMS_SUCCESSFUL;

    case HEAP_RESIZE_UNSATISFIED:
      status = RTEMS_UNSATISFIED;
      break;

    default:
      status = RTEMS_INVALID_ADDRESS;
      break;
  }

  _Region_Unlock( the_region );
  return status;
}
