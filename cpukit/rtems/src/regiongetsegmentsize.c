/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

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
 *  rtems_region_get_segment_size
 *
 *  This directive will return the size of the segment indicated
 *
 *  Input parameters:
 *    id         - region id
 *    segment    - segment address
 *    size       - pointer to segment size in bytes
 *
 *  Output parameters:
 *    size       - segment size in bytes filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_get_segment_size(
  Objects_Id         id,
  void              *segment,
  unsigned32        *size
)
{
  register Region_Control *the_region;
  Objects_Locations        location;
  Thread_Control          *executing;

  _RTEMS_Lock_allocator();
  executing  = _Thread_Executing;
  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      _RTEMS_Unlock_allocator();
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      _RTEMS_Unlock_allocator();
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:

      if ( _Heap_Size_of_user_area( &the_region->Memory, segment, size ) ) {
        _RTEMS_Unlock_allocator();
        return RTEMS_SUCCESSFUL;
      }
      _RTEMS_Unlock_allocator();
      return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
