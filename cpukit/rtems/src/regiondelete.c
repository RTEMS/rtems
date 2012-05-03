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

/*
 *  rtems_region_delete
 *
 *  This directive allows a thread to delete a region specified by
 *  the region identifier, provided that none of its segments are
 *  still allocated.
 *
 *  Input parameters:
 *    id - region id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_region_delete(
  rtems_id id
)
{
  Objects_Locations   location;
  rtems_status_code   return_status;
  Region_Control     *the_region;

  _RTEMS_Lock_allocator();

    the_region = _Region_Get( id, &location );
    switch ( location ) {

      case OBJECTS_LOCAL:
        _Region_Debug_Walk( the_region, 5 );
        if ( the_region->number_of_used_blocks != 0 )
          return_status = RTEMS_RESOURCE_IN_USE;
        else {
          _Objects_Close( &_Region_Information, &the_region->Object );
          _Region_Free( the_region );
          return_status = RTEMS_SUCCESSFUL;
        }
        break;

#if defined(RTEMS_MULTIPROCESSING)
      case OBJECTS_REMOTE:        /* this error cannot be returned */
        break;
#endif

      case OBJECTS_ERROR:
      default:
        return_status = RTEMS_INVALID_ID;
        break;
    }

  _RTEMS_Unlock_allocator();
  return return_status;
}
