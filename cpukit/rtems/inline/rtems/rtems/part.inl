/*  part.inl
 *
 *  This file contains the macro implementation of all inlined routines
 *  in the Partition Manager.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __PARTITION_inl
#define __PARTITION_inl

/*PAGE
 *
 *  _Partition_Allocate_buffer
 *
 */

STATIC INLINE void *_Partition_Allocate_buffer (
   Partition_Control *the_partition
)
{
  return _Chain_Get( &the_partition->Memory );
}

/*PAGE
 *
 *  _Partition_Free_buffer
 *
 */

STATIC INLINE void _Partition_Free_buffer (
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
 */

STATIC INLINE boolean _Partition_Is_buffer_on_boundary (
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
 */

STATIC INLINE boolean _Partition_Is_buffer_valid (
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
 */

STATIC INLINE boolean _Partition_Is_buffer_size_aligned (
   unsigned32 buffer_size
)
{
  return ((buffer_size % CPU_PARTITION_ALIGNMENT) == 0);
}

/*PAGE
 *
 *  _Partition_Allocate
 *
 */

STATIC INLINE Partition_Control *_Partition_Allocate ( void )
{
  return (Partition_Control *) _Objects_Allocate( &_Partition_Information );
}

/*PAGE
 *
 *  _Partition_Free
 *
 */

STATIC INLINE void _Partition_Free (
   Partition_Control *the_partition
)
{
  _Objects_Free( &_Partition_Information, &the_partition->Object );
}

/*PAGE
 *
 *  _Partition_Get
 *
 */

STATIC INLINE Partition_Control *_Partition_Get (
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
 */

STATIC INLINE boolean _Partition_Is_null (
   Partition_Control *the_partition
)
{
   return ( the_partition == NULL  );
}

#endif
/* end of include file */
