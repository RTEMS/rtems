/**
 *  @file
 *
 *  @brief RTEMS Get Region Information
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

rtems_status_code rtems_region_get_information(
  rtems_id                id,
  Heap_Information_block *the_info
)
{
  Region_Control *the_region;

  if ( the_info == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_region = _Region_Get_and_lock( id );

  if ( the_region == NULL ) {
    return RTEMS_INVALID_ID;
  }

  _Heap_Get_information( &the_region->Memory, the_info );

  _Region_Unlock( the_region );
  return RTEMS_SUCCESSFUL;
}
