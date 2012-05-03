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
 *  rtems_region_create
 *
 *  This directive creates a region of physical contiguous memory area
 *  from which variable sized segments can be allocated.
 *
 *  Input parameters:
 *    name             - user defined region name
 *    starting_address - physical start address of region
 *    length           - physical length in bytes
 *    page_size        - page size in bytes
 *    attribute_set    - region attributes
 *    id               - address of region id to set
 *
 *  Output parameters:
 *    id               - region id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_region_create(
  rtems_name          name,
  void               *starting_address,
  uintptr_t           length,
  uintptr_t           page_size,
  rtems_attribute     attribute_set,
  rtems_id           *id
)
{
  rtems_status_code  return_status;
  Region_Control    *the_region;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !starting_address )
    return RTEMS_INVALID_ADDRESS;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  _RTEMS_Lock_allocator();                      /* to prevent deletion */

    the_region = _Region_Allocate();

    if ( !the_region )
      return_status = RTEMS_TOO_MANY;

    else {

      the_region->maximum_segment_size = _Heap_Initialize(
        &the_region->Memory, starting_address, length, page_size
      );

      if ( !the_region->maximum_segment_size ) {
        _Region_Free( the_region );
        return_status = RTEMS_INVALID_SIZE;
      }

      else {

        the_region->starting_address      = starting_address;
        the_region->length                = length;
        the_region->page_size             = page_size;
        the_region->attribute_set         = attribute_set;
        the_region->number_of_used_blocks = 0;

        _Thread_queue_Initialize(
          &the_region->Wait_queue,
          _Attributes_Is_priority( attribute_set ) ?
             THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
          STATES_WAITING_FOR_SEGMENT,
          RTEMS_TIMEOUT
        );

        _Objects_Open(
          &_Region_Information,
          &the_region->Object,
          (Objects_Name) name
        );

        *id = the_region->Object.id;
        return_status = RTEMS_SUCCESSFUL;
      }
    }

  _RTEMS_Unlock_allocator();
  return return_status;
}
