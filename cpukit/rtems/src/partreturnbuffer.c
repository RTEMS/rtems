/*
 *  Partition Manager
 *
 *
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/partimpl.h>

rtems_status_code rtems_partition_return_buffer(
  rtems_id  id,
  void     *buffer
)
{
  Partition_Control *the_partition;
  ISR_lock_Context   lock_context;

  the_partition = _Partition_Get( id, &lock_context );

  if ( the_partition == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Partition_MP_Return_buffer( id, buffer );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _Partition_Acquire_critical( the_partition, &lock_context );

  if ( !_Partition_Is_buffer_valid( buffer, the_partition ) ) {
    _Partition_Release( the_partition, &lock_context );
    return RTEMS_INVALID_ADDRESS;
  }

  _Partition_Free_buffer( the_partition, buffer );
  the_partition->number_of_used_blocks -= 1;
  _Partition_Release( the_partition, &lock_context );
  return RTEMS_SUCCESSFUL;
}
