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

/*PAGE
 *
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
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_region_return_segment(
  Objects_Id  id,
  void       *segment
)
{
  register Region_Control *the_region;
  Thread_Control          *the_thread;
  Objects_Locations        location;
  void                   **the_segment;
#ifdef RTEMS_REGION_FREE_SHRED_PATTERN
  unsigned32               size;
#endif
  int                      status;

  _RTEMS_Lock_allocator();
  the_region = _Region_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:        /* this error cannot be returned */
      _RTEMS_Unlock_allocator();
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      _RTEMS_Unlock_allocator();
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:

      _Region_Debug_Walk( the_region, 3 );

#ifdef RTEMS_REGION_FREE_SHRED_PATTERN
      if ( _Heap_Size_of_user_area( &the_region->Memory, segment, &size ) ) {
        memset(segment, (RTEMS_REGION_FREE_SHRED_PATTERN & 0xFF), size);
      } else {
        _RTEMS_Unlock_allocator();
        return RTEMS_INVALID_ADDRESS;
      }
#endif

      status = _Region_Free_segment( the_region, segment );

      _Region_Debug_Walk( the_region, 4 );

      if ( !status ) {
        _RTEMS_Unlock_allocator();
        return RTEMS_INVALID_ADDRESS;
      }

      the_region->number_of_used_blocks -= 1;
      for ( ; ; ) {
        the_thread = _Thread_queue_First( &the_region->Wait_queue );

        if ( the_thread == NULL )
           break;

        the_segment = (void **) _Region_Allocate_segment(
           the_region, 
           the_thread->Wait.count
        );

        if ( the_segment == NULL )
           break;

        *(void **)the_thread->Wait.return_argument = the_segment;
        the_region->number_of_used_blocks += 1;
        _Thread_queue_Extract( &the_region->Wait_queue, the_thread );
        the_thread->Wait.return_code = RTEMS_SUCCESSFUL;
      }

      _RTEMS_Unlock_allocator();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
