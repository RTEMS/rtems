/**
 *  @file
 *
 *  @brief RTEMS Delete Partition
 *  @ingroup ClassicPart
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

#include <rtems/rtems/partimpl.h>
#include <rtems/rtems/attrimpl.h>

rtems_status_code rtems_partition_delete(
  rtems_id id
)
{
  Partition_Control *the_partition;
  Objects_Locations  location;
  ISR_lock_Context   lock_context;

  _Objects_Allocator_lock();
  the_partition = _Partition_Get( id, &location, &lock_context );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _Partition_Acquire_critical( the_partition, &lock_context );

      if ( the_partition->number_of_used_blocks == 0 ) {
        _Objects_Close( &_Partition_Information, &the_partition->Object );
#if defined(RTEMS_MULTIPROCESSING)
        if ( _Attributes_Is_global( the_partition->attribute_set ) ) {

          _Objects_MP_Close(
            &_Partition_Information,
            the_partition->Object.id
          );

          _Partition_MP_Send_process_packet(
            PARTITION_MP_ANNOUNCE_DELETE,
            the_partition->Object.id,
            0,                         /* Not used */
            0                          /* Not used */
          );
        }
#endif

        _Partition_Release( the_partition, &lock_context );
        _Partition_Destroy( the_partition );
        _Partition_Free( the_partition );
        _Objects_Allocator_unlock();
        return RTEMS_SUCCESSFUL;
      }

      _Partition_Release( the_partition, &lock_context );
      _Objects_Allocator_unlock();
      return RTEMS_RESOURCE_IN_USE;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Objects_Allocator_unlock();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  _Objects_Allocator_unlock();

  return RTEMS_INVALID_ID;
}
