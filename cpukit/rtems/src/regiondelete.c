/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

/*PAGE
 *
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
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_delete(
  Objects_Id id
)
{
  register Region_Control *the_region;
  Objects_Locations               location;

  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      _Region_Debug_Walk( the_region, 5 );
      if ( the_region->number_of_used_blocks == 0 ) {
        _Objects_Close( &_Region_Information, &the_region->Object );
        _Region_Free( the_region );
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_RESOURCE_IN_USE;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
