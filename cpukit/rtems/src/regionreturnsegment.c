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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/regionimpl.h>

rtems_status_code rtems_region_return_segment(
  rtems_id  id,
  void     *segment
)
{
  Region_Control *the_region;

  the_region = _Region_Get_and_lock( id );

  if ( the_region == NULL ) {
    return RTEMS_INVALID_ID;
  }

  if ( _Region_Free_segment( the_region, segment ) ) {
    /* Unlocks allocator */
    _Region_Process_queue( the_region );
    return RTEMS_SUCCESSFUL;
  }

  _Region_Unlock( the_region );
  return RTEMS_INVALID_ADDRESS;
}
