/*  part.inl
 *
 *  This file contains the macro implementation of all inlined routines
 *  in the Partition Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __PARTITION_inl
#define __PARTITION_inl

/*PAGE
 *
 *  _Partition_Allocate_buffer
 *
 *  DESCRIPTION:
 *
 *  This function attempts to allocate a buffer from the_partition.
 *  If successful, it returns the address of the allocated buffer.
 *  Otherwise, it returns NULL.
 */

RTEMS_INLINE_ROUTINE void *_Partition_Allocate_buffer (
   Partition_Control *the_partition
)
{
  return _Chain_Get( &the_partition->Memory );
}

/*PAGE
 *
 *  _Partition_Free_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine frees the_buffer to the_partition.
 */

RTEMS_INLINE_ROUTINE void _Partition_Free_buffer (
  Partition_Control *the_partition,
  Chain_Node        *the_buffer
)
{
  _Chain_Append( &the_partition->Memory, the_buffer );
}

/*PAGE
 *
 *  _Partition_Is_buffer_on_boundary
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_buffer is on a valid buffer
 *  boundary for the_partition, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Partition_Is_buffer_on_boundary (
  void              *the_buffer,
  Partition_Control *the_partition
)
{
  unsigned32   offset;

  offset = (unsigned32) _Addresses_Subtract(
    the_buffer,
    the_partition->starting_address
  );

  return ((offset % the_partition->buffer_size) == 0);
}

/*PAGE
 *
 *  _Partition_Is_buffer_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_buffer is a valid buffer from
 *  the_partition, otherwise FALSE is returned.
 */

RTEMS_INLINE_ROUTINE boolean _Partition_Is_buffer_valid (
   Chain_Node        *the_buffer,
   Partition_Control *the_partition
)
{
  void *starting;
  void *ending;

  starting = the_partition->starting_address;
  ending   = _Addresses_Add_offset( starting, the_partition->length );

  return (
    _Addresses_Is_in_range( the_buffer, starting, ending ) &&
    _Partition_Is_buffer_on_boundary( the_buffer, the_partition )
  );
}

/*PAGE
 *
 *  _Partition_Is_buffer_size_aligned
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the use of the specified buffer_size
 *  will result in the allocation of buffers whose first byte is
 *  properly aligned, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Partition_Is_buffer_size_aligned (
   unsigned32 buffer_size
)
{
  return ((buffer_size % CPU_PARTITION_ALIGNMENT) == 0);
}

/*PAGE
 *
 *  _Partition_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a partition control block from
 *  the inactive chain of free partition control blocks.
 */

RTEMS_INLINE_ROUTINE Partition_Control *_Partition_Allocate ( void )
{
  return (Partition_Control *) _Objects_Allocate( &_Partition_Information );
}

/*PAGE
 *
 *  _Partition_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a partition control block to the
 *  inactive chain of free partition control blocks.
 */

RTEMS_INLINE_ROUTINE void _Partition_Free (
   Partition_Control *the_partition
)
{
  _Objects_Free( &_Partition_Information, &the_partition->Object );
}

/*PAGE
 *
 *  _Partition_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps partition IDs to partition control blocks.
 *  If ID corresponds to a local partition, then it returns
 *  the_partition control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  If the partition ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_partition is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_partition is undefined.
 */

RTEMS_INLINE_ROUTINE Partition_Control *_Partition_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Partition_Control *)
    _Objects_Get( &_Partition_Information, id, location );
}

/*PAGE
 *
 *  _Partition_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_partition is NULL
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Partition_Is_null (
   Partition_Control *the_partition
)
{
   return ( the_partition == NULL  );
}

#endif
/* end of include file */
