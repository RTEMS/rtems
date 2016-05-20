/**
 *  @file
 *
 *  @brief RTEMS Get Partition Buffer
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

rtems_status_code rtems_partition_get_buffer(
  rtems_id   id,
  void     **buffer
)
{
  Partition_Control *the_partition;
  ISR_lock_Context   lock_context;
  void              *the_buffer;

  if ( buffer == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_partition = _Partition_Get( id, &lock_context );

  if ( the_partition == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Partition_MP_Get_buffer( id, buffer );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _Partition_Acquire_critical( the_partition, &lock_context );
  the_buffer = _Partition_Allocate_buffer( the_partition );

  if ( the_buffer == NULL ) {
    _Partition_Release( the_partition, &lock_context );
    return RTEMS_UNSATISFIED;
  }

  the_partition->number_of_used_blocks += 1;
  _Partition_Release( the_partition, &lock_context );
  *buffer = the_buffer;
  return RTEMS_SUCCESSFUL;
}
