/**
 * @file
 *
 * @ingroup ClassicPart Partitions
 *
 * @brief RTEMS Partition Create
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
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/sysstate.h>
#include <rtems/sysinit.h>


rtems_status_code rtems_partition_create(
  rtems_name       name,
  void            *starting_address,
  uintptr_t        length,
  size_t           buffer_size,
  rtems_attribute  attribute_set,
  rtems_id        *id
)
{
  Partition_Control *the_partition;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !starting_address )
    return RTEMS_INVALID_ADDRESS;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  if ( length == 0 )
    return RTEMS_INVALID_SIZE;

  if ( buffer_size == 0 )
    return RTEMS_INVALID_SIZE;

  if ( length < buffer_size )
    return RTEMS_INVALID_SIZE;

  if ( !_Partition_Is_buffer_size_aligned( buffer_size ) )
    return RTEMS_INVALID_SIZE;

  if ( buffer_size < sizeof( Chain_Node ) )
    return RTEMS_INVALID_SIZE;

  if ( !_Partition_Is_buffer_area_aligned( starting_address ) )
    return RTEMS_INVALID_ADDRESS;

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_System_state_Is_multiprocessing ) {
    attribute_set = _Attributes_Clear( attribute_set, RTEMS_GLOBAL );
  }
#endif

  the_partition = _Partition_Allocate();

  if ( !the_partition ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( attribute_set ) &&
       !( _Objects_MP_Allocate_and_open( &_Partition_Information, name,
                            the_partition->Object.id, false ) ) ) {
    _Partition_Free( the_partition );
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }
#endif

  _Partition_Initialize(
    the_partition,
    starting_address,
    length,
    buffer_size,
    attribute_set
  );

  _Objects_Open(
    &_Partition_Information,
    &the_partition->Object,
    (Objects_Name) name
  );

  *id = the_partition->Object.id;
#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( attribute_set ) )
    _Partition_MP_Send_process_packet(
      PARTITION_MP_ANNOUNCE_CREATE,
      the_partition->Object.id,
      name,
      0                  /* Not used */
    );
#endif

  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Partition_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Partition_Information );
}

RTEMS_SYSINIT_ITEM(
  _Partition_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_PARTITION,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
