/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This source file contains the implementation of
 *   rtems_partition_return_buffer().
 */

/*
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
#include <rtems/score/address.h>
#include <rtems/score/chainimpl.h>

static bool _Partition_Is_address_on_buffer_boundary(
  const Partition_Control *the_partition,
  const void              *the_buffer
)
{
  intptr_t offset;

  offset = _Addresses_Subtract(
    the_buffer,
    the_partition->starting_address
  );

  return ( offset % the_partition->buffer_size ) == 0;
}

static bool _Partition_Is_address_a_buffer_begin(
   const Partition_Control *the_partition,
   const void              *the_buffer
)
{
  void *starting;
  void *ending;

  starting = the_partition->starting_address;
  ending   = _Addresses_Add_offset( starting, the_partition->length );

  return _Addresses_Is_in_range( the_buffer, starting, ending )
    && _Partition_Is_address_on_buffer_boundary( the_partition, the_buffer );
}

static void _Partition_Free_buffer(
  Partition_Control *the_partition,
  void              *the_buffer
)
{
  _Chain_Append_unprotected( &the_partition->Memory, the_buffer );
}

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

  if ( !_Partition_Is_address_a_buffer_begin( the_partition, buffer ) ) {
    _Partition_Release( the_partition, &lock_context );
    return RTEMS_INVALID_ADDRESS;
  }

  _Partition_Free_buffer( the_partition, buffer );
  the_partition->number_of_used_blocks -= 1;
  _Partition_Release( the_partition, &lock_context );
  return RTEMS_SUCCESSFUL;
}
