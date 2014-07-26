/**
 * @file
 *
 * @ingroup ClassicPartImpl
 *
 * @brief Classic Partition Manager Implementation
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_PARTIMPL_H
#define _RTEMS_RTEMS_PARTIMPL_H

#include <rtems/rtems/part.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicPartImpl Classic Partition Manager Implementation
 *
 * @ingroup ClassicPart
 *
 * @{
 */

/**
 * This constant is defined to extern most of the time when using
 * this header file. However by defining it to nothing, the data
 * declared in this header file can be instantiated. This is done
 * in a single per manager file.
 *
 * Partition Manager -- Instantiate Data
 */
#ifndef RTEMS_PART_EXTERN
#define RTEMS_PART_EXTERN extern
#endif

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
RTEMS_PART_EXTERN Objects_Information _Partition_Information;

/**
 *  @brief Partition Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Partition_Manager_initialization(void);

/**
 *  @brief Allocate a buffer from the_partition.
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

/**
 *  @brief Frees the_buffer to the_partition.
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

/**
 *  @brief Checks whether is on a valid buffer boundary for the_partition.
 *
 *  This function returns TRUE if the_buffer is on a valid buffer
 *  boundary for the_partition, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Partition_Is_buffer_on_boundary (
  void              *the_buffer,
  Partition_Control *the_partition
)
{
  uint32_t     offset;

  offset = (uint32_t) _Addresses_Subtract(
    the_buffer,
    the_partition->starting_address
  );

  return ((offset % the_partition->buffer_size) == 0);
}

/**
 *  @brief Checks whether the_buffer is a valid buffer from the_partition.
 *
 *  This function returns TRUE if the_buffer is a valid buffer from
 *  the_partition, otherwise FALSE is returned.
 */
RTEMS_INLINE_ROUTINE bool _Partition_Is_buffer_valid (
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

/**
 *  @brief Checks if partition is buffer size aligned.
 *
 *  This function returns TRUE if the use of the specified buffer_size
 *  will result in the allocation of buffers whose first byte is
 *  properly aligned, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Partition_Is_buffer_size_aligned (
   uint32_t   buffer_size
)
{
  return ((buffer_size % CPU_PARTITION_ALIGNMENT) == 0);
}

/**
 *  @brief Allocates a partition control block from the
 *  inactive chain of free partition control blocks.
 *
 *  This function allocates a partition control block from
 *  the inactive chain of free partition control blocks.
 */
RTEMS_INLINE_ROUTINE Partition_Control *_Partition_Allocate ( void )
{
  return (Partition_Control *) _Objects_Allocate( &_Partition_Information );
}

/**
 *  @brief Frees a partition control block to the
 *  inactive chain of free partition control blocks.
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

/**
 *  @brief Maps partition IDs to partition control blocks.
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

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/partmp.h>
#endif

#endif
/* end of include file */
