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
#include <rtems/score/apimutex.h>

/*PAGE
 *
 *  rtems_region_extend
 *
 *  This directive attempts to grow a region of physical contiguous memory area
 *  from which variable sized segments can be allocated.
 *
 *  Input parameters:
 *    id         - id of region to grow
 *    start      - starting address of memory area for extension
 *    length     - physical length in bytes to grow the region
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_region_extend(
  Objects_Id          id,
  void               *starting_address,
  unsigned32          length
)
{
  Region_Control     *the_region;
  Objects_Locations   location;
  unsigned32          amount_extended;
  Heap_Extend_status  heap_status;
  rtems_status_code   status;

  status = RTEMS_SUCCESSFUL;

  _RTEMS_Lock_allocator();                      /* to prevent deletion */
  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      _RTEMS_Unlock_allocator();
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      _RTEMS_Unlock_allocator();
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:

      heap_status = _Heap_Extend(
        &the_region->Memory,
        starting_address,
        length,
        &amount_extended
      );

      switch ( heap_status ) {
        case HEAP_EXTEND_SUCCESSFUL:
          the_region->length                += amount_extended;
          the_region->maximum_segment_size  += amount_extended;
          break;
        case HEAP_EXTEND_ERROR:
          status = RTEMS_INVALID_ADDRESS;
          break;
        case HEAP_EXTEND_NOT_IMPLEMENTED:
          status = RTEMS_NOT_IMPLEMENTED;
          break;
      }
      _RTEMS_Unlock_allocator();
      return( status );
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
