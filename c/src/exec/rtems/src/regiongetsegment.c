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
 *  rtems_region_get_segment
 *
 *  This directive will obtain a segment from the given region.
 *
 *  Input parameters:
 *    id         - region id
 *    size       - segment size in bytes
 *    option_set - wait option
 *    timeout    - number of ticks to wait (0 means wait forever)
 *    segment    - pointer to segment address
 *
 *  Output parameters:
 *    segment    - pointer to segment address filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_region_get_segment(
  Objects_Id         id,
  unsigned32         size,
  rtems_option       option_set,
  rtems_interval     timeout,
  void              **segment
)
{
  register Region_Control *the_region;
  Objects_Locations        location;
  Thread_Control          *executing;
  void                    *the_segment;

  *segment = NULL;

  if ( size == 0 )
    return RTEMS_INVALID_SIZE;

  _RTEMS_Lock_allocator();
  executing  = _Thread_Executing;
  the_region = _Region_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:        /* this error cannot be returned */
      _RTEMS_Unlock_allocator();
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      _RTEMS_Unlock_allocator();
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( size > the_region->maximum_segment_size ) {
        _RTEMS_Unlock_allocator();
        return RTEMS_INVALID_SIZE;
      }

      _Region_Debug_Walk( the_region, 1 );

      the_segment = _Region_Allocate_segment( the_region, size );

      _Region_Debug_Walk( the_region, 2 );

      if ( the_segment ) {
        the_region->number_of_used_blocks += 1;
        _RTEMS_Unlock_allocator();
        *segment = the_segment;
        return RTEMS_SUCCESSFUL;
      }

      if ( _Options_Is_no_wait( option_set ) ) {
        _RTEMS_Unlock_allocator();
        return RTEMS_UNSATISFIED;
      }

      executing->Wait.queue           = &the_region->Wait_queue;
      executing->Wait.id              = id;
      executing->Wait.count           = size;
      executing->Wait.return_argument = (unsigned32 *) segment;

      _Thread_queue_Enter_critical_section( &the_region->Wait_queue );

      _Thread_queue_Enqueue( &the_region->Wait_queue, timeout );

      _RTEMS_Unlock_allocator();
      return (rtems_status_code) executing->Wait.return_code;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
