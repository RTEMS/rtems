/**
 *  @file
 *
 *  @brief RTEMS Get Partition Buffer
 *  @ingroup ClassicPart
 */

/*
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
#include <rtems/rtems/partimpl.h>
#include <rtems/score/thread.h>

rtems_status_code rtems_partition_get_buffer(
  rtems_id   id,
  void     **buffer
)
{
  register Partition_Control *the_partition;
  Objects_Locations           location;
  void                       *the_buffer;

  if ( !buffer )
    return RTEMS_INVALID_ADDRESS;

  the_partition = _Partition_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      the_buffer = _Partition_Allocate_buffer( the_partition );
      if ( the_buffer ) {
        the_partition->number_of_used_blocks += 1;
        _Objects_Put( &the_partition->Object );
        *buffer = the_buffer;
        return RTEMS_SUCCESSFUL;
      }
      _Objects_Put( &the_partition->Object );
      return RTEMS_UNSATISFIED;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Executing->Wait.return_argument = buffer;
      return(
        _Partition_MP_Send_request_packet(
          PARTITION_MP_GET_BUFFER_REQUEST,
          id,
          0           /* Not used */
        )
      );
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
