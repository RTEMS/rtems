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
#include <rtems/score/apimutex.h>
#include <rtems/score/thread.h>

/*
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
  rtems_id                id,
  Heap_Information_block *the_info
)
{
  Objects_Locations        location;
  rtems_status_code        return_status;
  register Region_Control *the_region;

  if ( !the_info )
    return RTEMS_INVALID_ADDRESS;

  _RTEMS_Lock_allocator();

    the_region = _Region_Get( id, &location );
    switch ( location ) {

      case OBJECTS_LOCAL:
        _Heap_Get_information( &the_region->Memory, the_info );
        return_status = RTEMS_SUCCESSFUL;
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
