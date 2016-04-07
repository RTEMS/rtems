/**
 *  @file
 *
 *  @brief RTEMS Return Region Segment
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

rtems_status_code rtems_region_return_segment(
  rtems_id  id,
  void     *segment
)
{
  rtems_status_code  status;
  Region_Control    *the_region;

  _RTEMS_Lock_allocator();

  the_region = _Region_Get( id );

  if ( the_region != NULL ) {
     if ( _Region_Free_segment( the_region, segment ) ) {
       the_region->number_of_used_blocks -= 1;

       /* Unlocks allocator */
       _Region_Process_queue( the_region );
       return RTEMS_SUCCESSFUL;
     } else {
       status = RTEMS_INVALID_ADDRESS;
     }
  } else {
    status = RTEMS_INVALID_ID;
  }

  _RTEMS_Unlock_allocator();
  return status;
}
