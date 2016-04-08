/**
 *  @file
 *
 *  @brief RTEMS Delete Region
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/regionimpl.h>

rtems_status_code rtems_region_delete(
  rtems_id id
)
{
  rtems_status_code  status;
  Region_Control    *the_region;

  _Objects_Allocator_lock();

  the_region = _Region_Get_and_lock( id );

  if ( the_region == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_INVALID_ID;
  }

  _Heap_Protection_free_all_delayed_blocks( &the_region->Memory );

  if ( the_region->Memory.stats.used_blocks != 0 ) {
    status = RTEMS_RESOURCE_IN_USE;
  } else {
    _Objects_Close( &_Region_Information, &the_region->Object );
    _Region_Free( the_region );
    status = RTEMS_SUCCESSFUL;
  }

  _Region_Unlock( the_region );
  _Objects_Allocator_unlock();
  return status;
}
