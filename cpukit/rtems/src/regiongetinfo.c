/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
 *  rtems_region_get_information
 *
 *  This directive will return information about the region specified.
 *
 *  Input parameters:
 *    id         - region id
 *    the_info   - pointer to region information block
 *
 *  Output parameters:
 *    *the_info   - region information block filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_get_information(
  Objects_Id              id,
  Heap_Information_block *the_info
)
{
  register Region_Control *the_region;
  Objects_Locations        location;

  if ( !the_info )
    return RTEMS_INVALID_ADDRESS;

  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:

      if ( _Heap_Get_information( &the_region->Memory, the_info ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
