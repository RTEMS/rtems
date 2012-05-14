/*
 *  Partition Manager
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/score/object.h>
#include <rtems/rtems/part.h>
#include <rtems/score/thread.h>
#include <rtems/score/sysstate.h>

/*
 *  rtems_partition_delete
 *
 *  This directive allows a thread to delete a partition specified by
 *  the partition identifier, provided that none of its buffers are
 *  still allocated.
 *
 *  Input parameters:
 *    id - partition id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_partition_delete(
  rtems_id id
)
{
  register Partition_Control *the_partition;
  Objects_Locations           location;

  the_partition = _Partition_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( the_partition->number_of_used_blocks == 0 ) {
        _Objects_Close( &_Partition_Information, &the_partition->Object );
        _Partition_Free( the_partition );
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

        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_RESOURCE_IN_USE;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
