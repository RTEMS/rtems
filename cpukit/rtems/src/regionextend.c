/**
 *  @file
 *
 *  @brief RTEMS Extend Region
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

rtems_status_code rtems_region_extend(
  rtems_id   id,
  void      *starting_address,
  uintptr_t  length
)
{
  rtems_status_code  status;
  Region_Control    *the_region;
  uintptr_t          amount_extended;

  if ( starting_address == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  _RTEMS_Lock_allocator();

  the_region = _Region_Get( id );

  if ( the_region != NULL ) {
    amount_extended = _Heap_Extend(
      &the_region->Memory,
      starting_address,
      length,
      0
    );

    if ( amount_extended > 0 ) {
      the_region->length               += amount_extended;
      the_region->maximum_segment_size += amount_extended;
      status = RTEMS_SUCCESSFUL;
    } else {
      status = RTEMS_INVALID_ADDRESS;
    }
  } else {
    status = RTEMS_INVALID_ID;
  }

  _RTEMS_Unlock_allocator();
  return status;
}
