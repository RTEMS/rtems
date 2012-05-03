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

#ifdef RTEMS_REGION_SHRED_ON_FREE
#include <string.h>

#ifndef RTEMS_REGION_FREE_SHRED_PATTERN
#define RTEMS_REGION_FREE_SHRED_PATTERN 0x00
#endif
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
 *  rtems_region_return_segment
 *
 *  This directive will return a segment to its region.
 *
 *  Input parameters:
 *    id      - region id
 *    segment - pointer to segment address
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_region_return_segment(
  rtems_id  id,
  void     *segment
)
{
  Objects_Locations        location;
  rtems_status_code        return_status;
#ifdef RTEMS_REGION_FREE_SHRED_PATTERN
  uint32_t                 size;
#endif
  int                      status;
  register Region_Control *the_region;

  _RTEMS_Lock_allocator();

    the_region = _Region_Get( id, &location );
    switch ( location ) {

      case OBJECTS_LOCAL:

        _Region_Debug_Walk( the_region, 3 );

#ifdef RTEMS_REGION_FREE_SHRED_PATTERN
        if ( !_Heap_Size_of_alloc_area( &the_region->Memory, segment, &size ) )
          return_status = RTEMS_INVALID_ADDRESS;
        else {
          memset( segment, (RTEMS_REGION_FREE_SHRED_PATTERN & 0xFF), size );
#endif
          status = _Region_Free_segment( the_region, segment );

          _Region_Debug_Walk( the_region, 4 );

          if ( !status )
            return_status = RTEMS_INVALID_ADDRESS;
          else {
            the_region->number_of_used_blocks -= 1;

            _Region_Process_queue(the_region); /* unlocks allocator */

            return RTEMS_SUCCESSFUL;
          }
#ifdef RTEMS_REGION_FREE_SHRED_PATTERN
        }
#endif
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
