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
#include <rtems/score/apimutex.h>
#include <rtems/score/thread.h>

/*PAGE
 *
 *  rtems_region_get_free_information
 *
 *  This directive will return information about the free blocks
 *  in the region specified.  Information about the used blocks
 *  will be returned as zero.
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

rtems_status_code rtems_region_get_free_information(
  Objects_Id              id,
  Heap_Information_block *the_info
)
{
  register Region_Control *the_region;
  Objects_Locations        location;

  if ( !the_info )
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

      the_info->Used.number   = 0;
      the_info->Used.total    = 0;
      the_info->Used.largest  = 0;

      _Heap_Get_free_information( &the_region->Memory, &the_info->Free );

      _RTEMS_Unlock_allocator();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
