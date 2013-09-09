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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/regionimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/apimutex.h>

rtems_status_code rtems_region_extend(
  rtems_id   id,
  void      *starting_address,
  uintptr_t  length
)
{
  uintptr_t           amount_extended;
  Objects_Locations   location;
  rtems_status_code   return_status;
  Region_Control     *the_region;

  if ( !starting_address )
    return RTEMS_INVALID_ADDRESS;

  _RTEMS_Lock_allocator();                      /* to prevent deletion */

    the_region = _Region_Get( id, &location );
    switch ( location ) {

      case OBJECTS_LOCAL:

        amount_extended = _Heap_Extend(
          &the_region->Memory,
          starting_address,
          length,
          0
        );

        if ( amount_extended > 0 ) {
          the_region->length                += amount_extended;
          the_region->maximum_segment_size  += amount_extended;
          return_status = RTEMS_SUCCESSFUL;
        } else {
          return_status = RTEMS_INVALID_ADDRESS;
        }
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
