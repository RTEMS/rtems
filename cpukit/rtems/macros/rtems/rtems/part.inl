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
 */

#define _Partition_Allocate_buffer( _the_partition ) \
  _Chain_Get( &(_the_partition)->Memory )

/*PAGE
 *
 *  _Partition_Free_buffer
 *
 */

#define _Partition_Free_buffer( _the_partition, _the_buffer ) \
  _Chain_Append( &(_the_partition)->Memory, (_the_buffer) )

/*PAGE
 *
 *  _Partition_Is_buffer_on_boundary
 *
 */

#define _Partition_Is_buffer_on_boundary( _the_buffer, _the_partition ) \
  ((((unsigned32) _Addresses_Subtract( \
                    (_the_buffer), \
                    (_the_partition)->starting_address ) \
                  ) % \
                  (_the_partition)->buffer_size) == 0)

/*PAGE
 *
 *  _Partition_Is_buffer_valid
 *
 */

#define _Partition_Is_buffer_valid( _the_buffer, _the_partition ) \
  ( \
    _Addresses_Is_in_range( \
      (_the_buffer), \
      (_the_partition)->starting_address, \
      _Addresses_Add_offset( \
         (_the_partition)->starting_address, \
         (_the_partition)->length \
      ) \
    ) && \
    _Partition_Is_buffer_on_boundary( (_the_buffer), (_the_partition) ) \
  )

/*PAGE
 *
 *  _Partition_Is_buffer_size_aligned
 *
 */

#define _Partition_Is_buffer_size_aligned( _buffer_size ) \
  ((_buffer_size) % CPU_PARTITION_ALIGNMENT == 0)

/*PAGE
 *
 *  _Partition_Allocate
 *
 */

#define _Partition_Allocate() \
  (Partition_Control *) _Objects_Allocate( &_Partition_Information )

/*PAGE
 *
 *  _Partition_Free
 *
 */

#define _Partition_Free( _the_partition ) \
  _Objects_Free( &_Partition_Information, &(_the_partition)->Object )

/*PAGE
 *
 *  _Partition_Get
 *
 */

#define _Partition_Get( _id, _location ) \
  (Partition_Control *) \
    _Objects_Get( &_Partition_Information, (_id), (_location) )

/*PAGE
 *
 *  _Partition_Is_null
 *
 */

#define _Partition_Is_null( _the_partition ) \
   ( (_the_partition) == NULL  )

#endif
/* end of include file */
