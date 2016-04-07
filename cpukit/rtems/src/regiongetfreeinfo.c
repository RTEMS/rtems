/**
 *  @file
 *
 *  @brief RTEMS Get Region Free Information
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#include <string.h>

rtems_status_code rtems_region_get_free_information(
  rtems_id                id,
  Heap_Information_block *the_info
)
{
  rtems_status_code  status;
  Region_Control    *the_region;

  if ( the_info == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  _RTEMS_Lock_allocator();

  the_region = _Region_Get( id );

  if ( the_region != NULL ) {
    memset( &the_info->Used, 0, sizeof( the_info->Used ) );
    _Heap_Get_free_information( &the_region->Memory, &the_info->Free );
    status = RTEMS_SUCCESSFUL;
  } else {
    status = RTEMS_INVALID_ID;
  }

  _RTEMS_Unlock_allocator();
  return status;
}
