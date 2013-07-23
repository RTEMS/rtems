/**
 *  @file
 *
 *  @brief RTEMS Get Region Segment Size
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/regionimpl.h>
#include <rtems/score/states.h>
#include <rtems/score/apimutex.h>

rtems_status_code rtems_region_get_segment_size(
  rtems_id   id,
  void      *segment,
  uintptr_t *size
)
{
  Objects_Locations        location;
  rtems_status_code        return_status = RTEMS_SUCCESSFUL;
  register Region_Control *the_region;

  if ( !segment )
    return RTEMS_INVALID_ADDRESS;

  if ( !size )
    return RTEMS_INVALID_ADDRESS;

  _RTEMS_Lock_allocator();

    the_region = _Region_Get( id, &location );
    switch ( location ) {

      case OBJECTS_LOCAL:
        if ( !_Heap_Size_of_alloc_area( &the_region->Memory, segment, size ) )
          return_status = RTEMS_INVALID_ADDRESS;
        break;

#if defined(RTEMS_MULTIPROCESSING)
      case OBJECTS_REMOTE:        /* this error cannot be returned */
        break;
#endif

      case OBJECTS_ERROR:
        return_status = RTEMS_INVALID_ID;
        break;
    }

  _RTEMS_Unlock_allocator();
  return return_status;
}
