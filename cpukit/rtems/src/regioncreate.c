/**
 * @file
 *
 * @ingroup ClassicRegion Regions
 *
 * @brief rtems_region_create
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/regionimpl.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/sysinit.h>

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

  the_region = _Region_Allocate();

    if ( !the_region )
      return_status = RTEMS_TOO_MANY;

    else {
      _Thread_queue_Object_initialize( &the_region->Wait_queue );

      if ( _Attributes_Is_priority( attribute_set ) ) {
        the_region->wait_operations = &_Thread_queue_Operations_priority;
      } else {
        the_region->wait_operations = &_Thread_queue_Operations_FIFO;
      }

      the_region->maximum_segment_size = _Heap_Initialize(
        &the_region->Memory, starting_address, length, page_size
      );

      if ( !the_region->maximum_segment_size ) {
        _Region_Free( the_region );
        return_status = RTEMS_INVALID_SIZE;
      } else {
        the_region->attribute_set = attribute_set;

        _Objects_Open(
          &_Region_Information,
          &the_region->Object,
          (Objects_Name) name
        );

        *id = the_region->Object.id;
        return_status = RTEMS_SUCCESSFUL;
      }
    }

  _Objects_Allocator_unlock();

  return return_status;
}

static void _Region_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Region_Information );
}

RTEMS_SYSINIT_ITEM(
  _Region_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_REGION,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
