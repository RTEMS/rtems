/*
 *  Partition Manager
 *
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/score/object.h>
#include <rtems/rtems/part.h>
#include <rtems/score/thread.h>
#include <rtems/score/sysstate.h>

/*PAGE
 *
 *  _Partition_Manager_initialization
 *
 *  This routine initializes all partition manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_partitions - number of partitions to initialize
 *
 *  Output parameters:  NONE
 */

void _Partition_Manager_initialization(
  unsigned32 maximum_partitions
)
{
  _Objects_Initialize_information(
    &_Partition_Information,
    OBJECTS_RTEMS_PARTITIONS,
    TRUE,
    maximum_partitions,
    sizeof( Partition_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );

  /*
   *  Register the MP Process Packet routine.
   */

  _MPCI_Register_packet_processor(
    MP_PACKET_PARTITION,
    _Partition_MP_Process_packet
  );

}

/*PAGE
 *
 *  rtems_partition_create
 *
 *  This directive creates a partiton of fixed sized buffers from the
 *  given contiguous memory area.
 *
 *  Input parameters:
 *    name             - user defined partition name
 *    starting_address - physical start address of partition
 *    length           - physical length in bytes
 *    buffer_size      - size of buffers in bytes
 *    attribute_set    - partition attributes
 *    id               - pointer to partition id
 *
 *  Output parameters:
 *    id                - partition id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_partition_create(
  rtems_name          name,
  void               *starting_address,
  unsigned32          length,
  unsigned32          buffer_size,
  rtems_attribute  attribute_set,
  Objects_Id         *id
)
{
  register Partition_Control *the_partition;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( length == 0 || buffer_size == 0 || length < buffer_size ||
         !_Partition_Is_buffer_size_aligned( buffer_size ) )
    return RTEMS_INVALID_SIZE;

  if ( !_Addresses_Is_aligned( starting_address ) )
     return RTEMS_INVALID_ADDRESS;

  if ( _Attributes_Is_global( attribute_set ) && 
       !_System_state_Is_multiprocessing )
    return RTEMS_MP_NOT_CONFIGURED;

  _Thread_Disable_dispatch();               /* prevents deletion */

  the_partition = _Partition_Allocate();

  if ( !the_partition ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  if ( _Attributes_Is_global( attribute_set ) &&
       !( _Objects_MP_Allocate_and_open( &_Partition_Information, name,
                            the_partition->Object.id, FALSE ) ) ) {
    _Partition_Free( the_partition );
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }
  the_partition->starting_address      = starting_address;
  the_partition->length                = length;
  the_partition->buffer_size           = buffer_size;
  the_partition->attribute_set         = attribute_set;
  the_partition->number_of_used_blocks = 0;

  _Chain_Initialize( &the_partition->Memory, starting_address,
                        length / buffer_size, buffer_size );

  _Objects_Open( &_Partition_Information, &the_partition->Object, &name );

  *id = the_partition->Object.id;
  if ( _Attributes_Is_global( attribute_set ) )
    _Partition_MP_Send_process_packet(
      PARTITION_MP_ANNOUNCE_CREATE,
      the_partition->Object.id,
      name,
      0                  /* Not used */
    );

  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_partition_ident
 *
 *  This directive returns the system ID associated with
 *  the partition name.
 *
 *  Input parameters:
 *    name - user defined partition name
 *    node - node(s) to be searched
 *    id   - pointer to partition id
 *
 *  Output parameters:
 *    *id               - partition id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_partition_ident(
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
)
{
  Objects_Name_to_id_errors  status;

  status = _Objects_Name_to_id( &_Partition_Information, &name, node, id );

  return _Status_Object_name_errors_to_status[ status ];
}

/*PAGE
 *
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
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_partition_delete(
  Objects_Id id
)
{
  register Partition_Control *the_partition;
  Objects_Locations                  location;

  the_partition = _Partition_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    case OBJECTS_LOCAL:
      if ( the_partition->number_of_used_blocks == 0 ) {
        _Objects_Close( &_Partition_Information, &the_partition->Object );
        _Partition_Free( the_partition );
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

        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_RESOURCE_IN_USE;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_partition_get_buffer
 *
 *  This directive will obtain a buffer from a buffer partition.
 *
 *  Input parameters:
 *    id     - partition id
 *    buffer - pointer to buffer address
 *
 *  Output parameters:
 *    buffer            - pointer to buffer address filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_partition_get_buffer(
  Objects_Id   id,
  void       **buffer
)
{
  register Partition_Control *the_partition;
  Objects_Locations           location;
  void                       *the_buffer;

  the_partition = _Partition_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      _Thread_Executing->Wait.return_argument = buffer;
      return(
        _Partition_MP_Send_request_packet(
          PARTITION_MP_GET_BUFFER_REQUEST,
          id,
          0           /* Not used */
        )
      );
    case OBJECTS_LOCAL:
      the_buffer = _Partition_Allocate_buffer( the_partition );
      if ( the_buffer ) {
        the_partition->number_of_used_blocks += 1;
        _Thread_Enable_dispatch();
        *buffer = the_buffer;
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_UNSATISFIED;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_partition_return_buffer
 *
 *  This directive will return the given buffer to the specified
 *  buffer partition.
 *
 *  Input parameters:
 *    id     - partition id
 *    buffer - pointer to buffer address
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_partition_return_buffer(
  Objects_Id  id,
  void       *buffer
)
{
  register Partition_Control *the_partition;
  Objects_Locations           location;

  the_partition = _Partition_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      return(
        _Partition_MP_Send_request_packet(
          PARTITION_MP_RETURN_BUFFER_REQUEST,
          id,
          buffer
        )
      );
    case OBJECTS_LOCAL:
      if ( _Partition_Is_buffer_valid( buffer, the_partition ) ) {
        _Partition_Free_buffer( the_partition, buffer );
        the_partition->number_of_used_blocks -= 1;
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
