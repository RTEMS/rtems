/*
 *  Region Manager
 *
 *
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
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/region.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/apimutex.h>

/*PAGE
 *
 *  rtems_region_resize_segment
 *
 *  This directive will try to resize segment to the new size 'size'
 *  "in place".
 *
 *  Input parameters:
 *    id      - region id
 *    segment - pointer to segment address
 *    size    - new required size
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_region_resize_segment(
  Objects_Id  id,
  void       *segment,
  size_t      size,
  size_t     *old_size
)
{
  register Region_Control *the_region;
  Objects_Locations        location;
  Heap_Resize_status       status;
  uint32_t                 avail_size;
  uint32_t                 osize;

  if ( !old_size )
    return RTEMS_INVALID_ADDRESS;

  _RTEMS_Lock_allocator();
  the_region = _Region_Get( id, &location );
  switch ( location ) {

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      _RTEMS_Unlock_allocator();
      return RTEMS_INTERNAL_ERROR;
#endif

    case OBJECTS_ERROR:
      _RTEMS_Unlock_allocator();
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:

      _Region_Debug_Walk( the_region, 7 );

      status = _Heap_Resize_block(
        &the_region->Memory,
        segment,
        (uint32_t) size,
        &osize,
        &avail_size
      );
      *old_size = (uint32_t) osize;

      _Region_Debug_Walk( the_region, 8 );

      if( status == HEAP_RESIZE_SUCCESSFUL && avail_size > 0 )
        _Region_Process_queue( the_region ); /* unlocks allocator internally */
      else
        _RTEMS_Unlock_allocator();

      return
        (status == HEAP_RESIZE_SUCCESSFUL) ?  RTEMS_SUCCESSFUL :
        (status == HEAP_RESIZE_UNSATISFIED) ? RTEMS_UNSATISFIED :
        RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
